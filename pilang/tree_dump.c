#include "tree_dump.h"

#include "mstring.h"
#include <stdio.h>
#include <string.h>

static void
tree_dump_impl(ast_node_base_t *root, uint16_t parent, int nth_child) {
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
    tree_dump_impl(((ast_schild_t*)root)->child, root->node_uid, 0);
    break;

  case ANK_SINGLE_CHILD_WDATA:
    tree_dump_impl(((ast_schild_wdata_t*)root)->child, 
                   root->node_uid, 0);
    break;

  case ANK_DUAL_CHILD:
    for (int i = 0; i < 2; i++) {
      tree_dump_impl(((ast_dchild_t*)root)->children[i],
                     root->node_uid, i);
    }
    break;

  case ANK_DUAL_CHILD_WDATA:
    for (int i = 0; i < 2; i++) {
      tree_dump_impl(((ast_dchild_wdata_t*)root)->children[i],
                     root->node_uid, i);
    }
    break;

  case ANK_TRIPLE_CHILD:
  	for (int i = 0; i < 3; i++) {
      tree_dump_impl(((ast_tchild_t*)root)->children[i],
                     root->node_uid, i);
    }
    break;

  case ANK_TRIPLE_CHILD_WDATA:
    for (int i = 0; i < 3; i++) {
      tree_dump_impl(((ast_tchild_wdata_t*)root)->children[i], 
                     root->node_uid, i);
    }
    break;

  case ANK_LIST: {
    ast_list_t *list = (ast_list_t*)root;
    int i = 0;
    for (iter_t it = list_begin(&(list->list)); 
         !iter_eq(it, list_end(&(list->list))); 
         it = iter_next(it)) {
      tree_dump_impl((ast_node_base_t*)iter_deref(it),
                     root->node_uid, i);
      i++;
    }
    break;
  }

  }
}

void tree_dump(ast_node_base_t *root) {
  tree_dump_impl(root, 0, 0);
}
