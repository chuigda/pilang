#include "ast.h"
#include "mstring.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NEW(TYPE) (TYPE*)malloc(sizeof(TYPE))

static uint16_t glob_uid_ = 0;

static uint16_t get_next_uid(void) {
  ++glob_uid_;
  return glob_uid_;
}

ast_node_base_t *leaf(ast_node_sema_t sema_info) {
  ast_leaf_t *ret = NEW(ast_leaf_t);
  ret->node_sema_info = sema_info;
  ret->node_kind = ANK_LEAF;
  ret->node_uid = get_next_uid();
  return (ast_node_base_t*)ret;
}

ast_node_base_t *leaf_wdata(ast_node_sema_t sema_info, jjvalue_t data) {
  ast_leaf_wdata_t *ret = NEW(ast_leaf_wdata_t);
  ret->node_kind = ANK_LEAF_WDATA;
  ret->node_sema_info = sema_info;
  ret->node_uid = get_next_uid();
  ret->data = data;
  return (ast_node_base_t*)ret;
}

ast_node_base_t *node1(ast_node_sema_t sema_info, 
                       ast_node_base_t *child) {
  ast_schild_t *ret = NEW(ast_schild_t);
  ret->node_kind = ANK_SINGLE_CHILD;
  ret->node_sema_info = sema_info;
  ret->node_uid = get_next_uid();
  ret->child = child;
  return (ast_node_base_t*)ret;
}

ast_node_base_t *node1_wdata(ast_node_sema_t sema_info, jjvalue_t data,
                             ast_node_base_t *child) {
  ast_schild_wdata_t *ret = NEW(ast_schild_wdata_t);
  ret->node_kind = ANK_SINGLE_CHILD_WDATA;
  ret->node_sema_info = sema_info;
  ret->node_uid = get_next_uid();
  ret->data = data;
  ret->child = child;
  return (ast_node_base_t*)ret;
}

ast_node_base_t *node2(ast_node_sema_t sema_info, 
                       ast_node_base_t *lchild, 
                       ast_node_base_t *rchild) {
  ast_dchild_t *ret = NEW(ast_dchild_t);
  ret->node_kind = ANK_DUAL_CHILD;
  ret->node_sema_info = sema_info;
  ret->node_uid = get_next_uid();
  ret->children[0] = lchild;
  ret->children[1] = rchild;
  return (ast_node_base_t*)ret;
}

ast_node_base_t *node2_wdata(ast_node_sema_t sema_info, jjvalue_t data,
                             ast_node_base_t *lchild,
                             ast_node_base_t *rchild) {
  ast_dchild_wdata_t *ret = NEW(ast_dchild_wdata_t);
  ret->node_kind = ANK_DUAL_CHILD_WDATA;
  ret->node_sema_info = sema_info;
  ret->node_uid = get_next_uid();
  ret->data = data;
  ret->children[0] = lchild;
  ret->children[1] = rchild;
  return (ast_node_base_t*)ret;
}

ast_node_base_t *node3(ast_node_sema_t sema_info, 
                       ast_node_base_t *child0, 
                       ast_node_base_t *child1,
                       ast_node_base_t *child2) {
  ast_tchild_t *ret = NEW(ast_tchild_t);
  ret->node_kind = ANK_TRIPLE_CHILD;
  ret->node_sema_info = sema_info;
  ret->node_uid = get_next_uid();
  ret->children[0] = child0;
  ret->children[1] = child1;
  ret->children[2] = child2;
  return (ast_node_base_t*)ret;
}

ast_node_base_t *node3_wdata(ast_node_sema_t sema_info, jjvalue_t data,
                             ast_node_base_t *child0,
                             ast_node_base_t *child1, 
                             ast_node_base_t *child2) {
  ast_tchild_wdata_t *ret = NEW(ast_tchild_wdata_t);
  ret->node_kind = ANK_TRIPLE_CHILD_WDATA;
  ret->node_sema_info = sema_info;
  ret->node_uid = get_next_uid();
  ret->data = data;
  ret->children[0] = child0;
  ret->children[1] = child1;
  ret->children[2] = child2;
  return (ast_node_base_t*)ret;
}

ast_node_base_t *node_list(ast_node_sema_t sema_info) {
  ast_list_t *ret = NEW(ast_list_t);
  ret->node_kind = ANK_LIST;
  ret->node_sema_info = sema_info;
  ret->node_uid = get_next_uid();
  create_list(&(ret->list), malloc, free);
  return (ast_node_base_t*)ret;
}

void ast_list_prepend(ast_node_base_t *node, ast_node_base_t *data) {
  assert(node->node_kind == ANK_LIST);
  ast_list_t *list = (ast_list_t*)node;
  list_push_front(&(list->list), data);
}

void ast_list_append(ast_node_base_t *node, ast_node_base_t *data) {
  assert(node->node_kind == ANK_LIST);
  ast_list_t *list = (ast_list_t*)node;
  list_push_back(&(list->list), data);
}

void tree_print(ast_node_base_t *root, uint16_t parent, int nth_child) {
  _Static_assert(ANS_COMMENCE_ == 0, "Incorrect ANS order!");
  _Static_assert(ANK_COMMENCE_ == 0, "Incorrect ANK order!");

  if (root == NULL) {
    printf("%u[%d] -> (nil)\n", (unsigned)parent, nth_child);
    return;
  }

  static const char* ans_strs[] = {
    #define ANS_DECLARE(KEY) #KEY ,
    #include "ast_sema_info.h"
    ""
  };

  static const char *ank_strs[] = {
    #define ANK_DECLARE(KEY) #KEY ,
    #include "ast_node_kind.h"
    ""
  };

  printf("%u[%d] -> NODE %u, ANK = %s, ANS = %s",
         (unsigned)parent,
         nth_child,
         (unsigned)root->node_uid,
         ank_strs[root->node_kind],
         ans_strs[root->node_sema_info]);

  if (strstr(ank_strs[root->node_kind], "WDATA")) {
    if (root->node_sema_info == ANS_ID
        || root->node_sema_info == ANS_IDREF
        || root->node_sema_info == ANS_FUNCTION) {
      printf(", SDATA = %s",
             get_string(((ast_node_wdata_base_t*)root)->data.svalue));
    }
    else {
      printf(", IDATA = %lld", 
             ((ast_node_wdata_base_t*)root)->data.ivalue);
    }
  }
  putchar('\n');

  switch (root->node_kind) {
  default:
  case ANK_LEAF:
  case ANK_LEAF_WDATA: 
    break;

  case ANK_SINGLE_CHILD:
    tree_print(((ast_schild_t*)root)->child, root->node_uid, 0);
    break;

  case ANK_SINGLE_CHILD_WDATA:
    tree_print(((ast_schild_wdata_t*)root)->child, root->node_uid, 0);
    break;

  case ANK_DUAL_CHILD:
    tree_print(((ast_dchild_t*)root)->children[0], root->node_uid, 0);
    tree_print(((ast_dchild_t*)root)->children[1], root->node_uid, 1);
    break;

  case ANK_DUAL_CHILD_WDATA:
    tree_print(((ast_dchild_wdata_t*)root)->children[0],
               root->node_uid, 0);
    tree_print(((ast_dchild_wdata_t*)root)->children[1],
               root->node_uid, 1);
    break;

  case ANK_TRIPLE_CHILD:
    tree_print(((ast_tchild_t*)root)->children[0], root->node_uid, 0);
    tree_print(((ast_tchild_t*)root)->children[1], root->node_uid, 1);
    tree_print(((ast_tchild_t*)root)->children[2], root->node_uid, 2);
    break;

  case ANK_TRIPLE_CHILD_WDATA:
    tree_print(((ast_tchild_wdata_t*)root)->children[0], 
               root->node_uid, 0);
    tree_print(((ast_tchild_wdata_t*)root)->children[1],
               root->node_uid, 1);
    tree_print(((ast_tchild_wdata_t*)root)->children[2], 
               root->node_uid, 2);
    break;
  }
}

