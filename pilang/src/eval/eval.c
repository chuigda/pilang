#include "eval.h"

#include "y.tab.h"
#include "stack.h"
#include "plheap.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum {
  PRVK_STKOBJ,
  PRVK_INT,
  PRVK_FLOAT,
  PRVK_STRING,
  PRVK_LIST,
  PRVK_REF,
  PRVK_NOTHING
} plregval_kind_t;

typedef struct {
  jjvalue_t value;
  plregval_kind_t prvk;
} plregval_t;

static ast_list_t *glob_program;
static plstack_t stack;

static plregval_t resolve_idref(int64_t name) {
  plregval_t ret;
  ret.prvk = PRVK_STKOBJ;
  ret.value.pvalue = stack_get(&stack, name);
  return ret;
}

static plregval_t deref(plobj_t *referred) {
  plregval_t ret;
  switch (referred->oid) {
  case OID_INT:
    ret.prvk = PRVK_INT;
    break;
  case OID_FLOAT:
    ret.prvk = PRVK_FLOAT;
    break;
  case OID_STR:
    ret.prvk = PRVK_STRING;
    break;
  case OID_LIST:
    ret.prvk = PRVK_LIST;
    break;
  default:
    assert(0 && "Unreachable");
  }
  ret.value = referred->value;
  return ret;
}

static plregval_t decay(plregval_t maybe_stkobj) {
  if (maybe_stkobj.prvk != PRVK_STKOBJ) {
    return maybe_stkobj;
  }
  plstkobj_t *obj = (plstkobj_t*)maybe_stkobj.value.pvalue;
  plregval_t ret;
  switch (obj->soid) {
  case SOID_INT:
    ret.prvk = PRVK_INT;
    ret.value.ivalue = obj->value.ivalue;
    break;
  case SOID_FLOAT:
    ret.prvk = PRVK_FLOAT;
    ret.value.fvalue = obj->value.fvalue;
    break;
  case SOID_UNDEFINED:
    ret.prvk = PRVK_NOTHING;
    break;
  case SOID_REF:
    ret = deref(((plobj_t*)obj)->value.pvalue);
    break;
  default:
    assert(0 && "Unreachable");
  }
  return ret;
}

static plregval_t eval_atom(ast_node_base_t *atom) {
  assert(atom->node_kind == ANK_LEAF_WDATA);
  ast_leaf_wdata_t *leaf = (ast_leaf_wdata_t*)atom;
  plregval_t ret;
  ret.value = leaf->data;
  switch (atom->node_sema_info) {
  case ANS_INTVAL:
    ret.prvk = PRVK_INT;
    break;
  case ANS_FLOATVAL:
    ret.prvk = PRVK_FLOAT;
    break;
  case ANS_IDREF:
    ret = resolve_idref(leaf->data.svalue);
    break;
  case ANS_STR:
    ret.prvk = PRVK_STRING;
    break;
  default:
    assert(0 && "Unreachable");
  }
  return ret;
}

static plregval_t eval_binexpr(ast_node_base_t *bin_expr) {
  assert(bin_expr->node_kind == ANK_DUAL_CHILD_WDATA);
  assert(bin_expr->node_sema_info == ANS_BINEXPR);
}

static plregval_t eval_func_call(ast_node_base_t *call_expr) {
  assert(call_expr->node_kind == ANK_DUAL_CHILD);
  assert(call_expr->node_sema_info == ANS_FUNC_CALL);
}

static plregval_t eval_expr(ast_node_base_t *expr) {
}

static plregval_t eval_if(ast_node_base_t *if_node) {
}

static plregval_t eval_while(ast_node_base_t *while_node) {
}

static plregval_t eval_for(ast_node_base_t *for_node) {
}

static plregval_t eval_return(ast_node_base_t *return_node) {
}

static plregval_t eval_stmt(ast_node_base_t *stmt) {
}

void eval_ast(ast_node_base_t *program) {
  assert(program->node_kind == ANK_LIST);
  assert(program->node_sema_info == ANS_FUNCTIONS);
  
  init_heap();
  init_stack(&stack);
  
  close_stack(&stack);
  close_heap();
}
