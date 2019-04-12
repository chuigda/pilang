#define EVAL_C
#include "eval.h"
#include "builtins.h"

#include "ast.h"
#include "util.h"
#include "y.tab.h"

#include <assert.h>
#include <float.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <stdlib.h>

static void asgn_attach_typeinfo(plvalue_t *obj, int16_t type) {
  obj->type = type;
  switch (obj->roc) {
    case ROC_ONSTACK: {
      stkobj_t *stkobj = (stkobj_t*)obj->value.pvalue;
      stkobj->soid = jt2soid(type);
      break;
    }
    case ROC_ONHEAP: {
      heapobj_t *heapobj = (heapobj_t*)obj->value.pvalue;
      heapobj->oid = jt2hoid(type);
      break;
    }
    default:
      UNREACHABLE;
  }
}

static void storage_precleanup(plvalue_t *obj) {
  if (obj->roc == ROC_ONHEAP) {
    destroy_object((heapobj_t*)(obj->value.pvalue));
  }
}

#define EITHER_IS(VALUETYPE, LHS, RHS) \
  ((LHS).type == VALUETYPE || (RHS).type == VALUETYPE)

static int64_t int_failsafe(result_t maybe) {
  return maybe.success ? maybe.value.ivalue : 0;
}

static double float_failsafe(result_t maybe) {
  return maybe.success ? maybe.value.fvalue : 0.0;
}

static strhdl_t str_failsafe(result_t maybe) {
  return maybe.success ? maybe.value.svalue
                       : create_string("undefined");
}

static bool bool_failsafe(result_t maybe) {
  return maybe.success ? maybe.value.bvalue
                       : false;
}

plvalue_t algebraic_calc(plvalue_t lhs, plvalue_t rhs,
                         algebraic_function_t alf) {
  lhs = auto_deref(lhs);
  rhs = auto_deref(rhs);

  if (alf == ALF_ADD) {
    if (EITHER_IS(JT_STR, lhs, rhs)) {
      const char* strl = get_string(str_failsafe(fetch_str(lhs)));
      const char* strr = get_string(str_failsafe(fetch_str(rhs)));
      char *temp = NEWN(char, strlen(strl) + strlen(strr) + 1);
      strcpy(temp, strl);
      strcat(temp, strr);
      int newstr = create_string(temp);
      free(temp);

      plvalue_t ret = create_temp();
      ret.type = JT_STR;
      ret.value.svalue = newstr;
      return ret;
    }
  }

  if (EITHER_IS(JT_FLOAT, lhs, rhs) && alf != ALF_MOD) {
    double f1 = float_failsafe(fetch_float(lhs));
    double f2 = float_failsafe(fetch_float(rhs));

    plvalue_t ret = create_temp();
    ret.type = JT_FLOAT;
    switch (alf) {
    case ALF_ADD: ret.value.fvalue = f1 + f2; break;
    case ALF_SUB: ret.value.fvalue = f1 - f2; break;
    case ALF_MUL: ret.value.fvalue = f1 * f2; break;
    case ALF_DIV: ret.value.fvalue = f1 / f2; break;
    default: UNREACHABLE
    }
    return ret;
  }
  else if (EITHER_IS(JT_INT, lhs, rhs) 
           || EITHER_IS(JT_BOOL, lhs, rhs)) {
    int64_t i1 = int_failsafe(fetch_int(lhs));
    int64_t i2 = int_failsafe(fetch_int(rhs));

    plvalue_t ret = create_temp();
    ret.type = JT_INT;
    switch (alf) {
    case ALF_ADD: ret.value.ivalue = i1 + i2; break;
    case ALF_SUB: ret.value.ivalue = i1 - i2; break;
    case ALF_MUL: ret.value.ivalue = i1 * i2; break;
    case ALF_DIV: ret.value.ivalue = i1 / i2; break;
    case ALF_MOD: ret.value.ivalue = i1 % i2; break;
    default: UNREACHABLE
    }
    return ret;
  }
  else {
    plvalue_t ret = create_temp();
    ret.type = JT_UNDEFINED;
    return ret;
  }
}

plvalue_t relative_calc(plvalue_t lhs, plvalue_t rhs,
                        relative_function_t rlf) {
  lhs = auto_deref(lhs);
  rhs = auto_deref(rhs);
  plvalue_t ret = create_temp();
  ret.type = JT_BOOL;

  if (EITHER_IS(JT_STR, lhs, rhs)) {
    const char* strl = get_string(str_failsafe(fetch_str(lhs)));
    const char* strr = get_string(str_failsafe(fetch_str(rhs)));
    int comp_result = strcmp(strl, strr);
    switch (rlf) {
    case RLF_EQ:  ret.value.bvalue = (comp_result == 0); break;
    case RLF_NEQ: ret.value.bvalue = (comp_result != 0); break;
    case RLF_LT:  ret.value.bvalue = (comp_result < 0); break;
    case RLF_GT:  ret.value.bvalue = (comp_result > 0); break;
    case RLF_NLT: ret.value.bvalue = (comp_result >= 0); break;
    case RLF_NGT: ret.value.bvalue = (comp_result <= 0); break;
    default: UNREACHABLE
    }
  }
  else if (EITHER_IS(JT_FLOAT, lhs, rhs)) {
    double f1 = float_failsafe(fetch_float(lhs));
    double f2 = float_failsafe(fetch_float(rhs));
    switch (rlf) {
    case RLF_EQ:
      ret.value.bvalue = (fabs(f1 - f2) < DBL_EPSILON); break;
    case RLF_NEQ:
      ret.value.bvalue = (fabs(f1 - f2) >= DBL_EPSILON); break;
    case RLF_LT:  ret.value.bvalue = f1 < f2;  break;
    case RLF_GT:  ret.value.bvalue = f1 > f2;  break;
    case RLF_NLT: ret.value.bvalue = f1 >= f2; break;
    case RLF_NGT: ret.value.bvalue = f1 <= f2; break;
    default: UNREACHABLE
    }
  }
  else if (EITHER_IS(JT_INT, lhs, rhs) 
           || EITHER_IS(JT_BOOL, lhs, rhs)) {
    int i1 = int_failsafe(fetch_int(lhs));
    int i2 = int_failsafe(fetch_int(rhs));
    switch (rlf) {
    case RLF_EQ:  ret.value.ivalue = i1 == i2; break;
    case RLF_NEQ: ret.value.ivalue = i1 != i2; break;
    case RLF_LT:  ret.value.bvalue = i1 < i2;  break;
    case RLF_GT:  ret.value.bvalue = i1 > i2;  break;
    case RLF_NLT: ret.value.bvalue = i1 >= i2; break;
    case RLF_NGT: ret.value.bvalue = i1 <= i2; break;
    default: UNREACHABLE
    }
  }
  else {
    ret.value.bvalue = false;
  }
  return ret;
}

plvalue_t logical_calc(ast_node_base_t *lhs, ast_node_base_t *rhs,
                       stack_t *stack, logical_function_t lgf) {
  plvalue_t ret = create_temp();
  ret.type = JT_BOOL;

  bool b1 =
    fetch_bool(auto_deref(eval_expr(lhs, stack))).value.bvalue;
  switch (lgf) {
  case LGF_AND:
    if (b1) {
      bool b2 =
        fetch_bool(auto_deref(eval_expr(rhs, stack))).value.bvalue;
      ret.value.bvalue = b1 && b2;
    }
    else {
      ret.value.bvalue = false;
    }
    break;
  case LGF_OR:
    if (b1) {
      ret.value.bvalue = true;
    }
    else {
      bool b2 =
        fetch_bool(auto_deref(eval_expr(rhs, stack))).value.bvalue;
      ret.value.bvalue = b1 || b2;
    }
    break;
  default: UNREACHABLE
  }

  return ret;
}

plvalue_t assign(plvalue_t lhs, plvalue_t rhs) {
  if (lhs.roc != ROC_ONSTACK && lhs.roc != ROC_ONHEAP) {
    return lhs;
  }

  plvalue_t lhs_org = create_temp();

  lhs_org.type = JT_UNDEFINED;
  if (lhs.roc == ROC_ONSTACK 
      && lhs.type == JT_REF 
      && rhs.type != JT_REF) {
    lhs = auto_deref(lhs);
    lhs_org.type = JT_REF;
    lhs_org.value.pvalue = fetch_storage(&lhs)->pvalue;
  }

  if (lhs.roc == ROC_ONHEAP) {
    rhs = auto_deref(rhs);
  }

  jjvalue_t *rhs_storage = fetch_storage(&rhs),
            *lhs_storage = fetch_storage(&lhs);

  if (lhs_storage == NULL) {
    return lhs;
  }

  storage_precleanup(&lhs);
  asgn_attach_typeinfo(&lhs, rhs.type);
  *lhs_storage = *rhs_storage;

  return lhs_org.type == JT_REF ? lhs_org : lhs;
}

plvalue_t eval_literal_expr(ast_leaf_wdata_t *node) {
  plvalue_t ret = create_temp();
  switch (node->node_sema_info) {
  case ANS_INTVAL:   ret.type = JT_INT;   break;
  case ANS_FLOATVAL: ret.type = JT_FLOAT; break;
  case ANS_STR:      ret.type = JT_STR;   break;
  case ANS_BOOLVAL:  ret.type = JT_BOOL;  break;
  }
  ret.value = node->value;
  return ret;
}

plvalue_t eval_idref_expr(ast_leaf_wdata_t *node, stack_t *stack) {
  return create_onstack(stack_get(stack, node->value.svalue));
}

plvalue_t eval_binexpr(ast_dchild_wdata_t *node, stack_t *stack) {
  ast_node_base_t *lhs_node = node->children[0];
  ast_node_base_t *rhs_node = node->children[1];

  switch (node->value.ivalue) {
  case TK_ESYM_AMPAMP:
    return logical_calc(lhs_node, rhs_node, stack, LGF_AND);
  case TK_ESYM_PIPEPIPE:
    return logical_calc(lhs_node, rhs_node, stack, LGF_OR);
  }

  plvalue_t lhs = eval_expr(lhs_node, stack);
  plvalue_t rhs = eval_expr(rhs_node, stack);
  if (node->value.ivalue == TK_ESYM_EQ) {
    return assign(lhs, rhs);
  }

  algebraic_function_t alf = ALF_INVL;
  relative_function_t rlf = RLF_INVL;
  switch (node->value.ivalue) {
  case TK_ESYM_PLUS:    alf = ALF_ADD; break;
  case TK_ESYM_MINUS:   alf = ALF_SUB; break;
  case TK_ESYM_ASTER:   alf = ALF_MUL; break;
  case TK_ESYM_SLASH:   alf = ALF_DIV; break;
  case TK_ESYM_PERCENT: alf = ALF_MOD; break;
  case TK_ESYM_LT:      rlf = RLF_LT;  break;
  case TK_ESYM_GT:      rlf = RLF_GT;  break;
  case TK_ESYM_LEQ:     rlf = RLF_NGT; break;
  case TK_ESYM_GEQ:     rlf = RLF_NLT; break;
  case TK_ESYM_NEQ:     rlf = RLF_NEQ; break;
  case TK_ESYM_EQEQ:    rlf = RLF_EQ; break;
  default: {
      UNREACHABLE
      plvalue_t failure = create_temp();
      failure.type = JT_UNDEFINED;
      return failure;
    }
  }
  if (alf != ALF_INVL) {
    return algebraic_calc(lhs, rhs, alf);
  }
  else {
    return relative_calc(lhs, rhs, rlf);
  }
}

plvalue_t eval_func_call(ast_dchild_t *func, stack_t *stack) {
  ast_leaf_wdata_t *idref = (ast_leaf_wdata_t*)(func->children[0]);
  ast_list_t *args = (ast_list_t*)(func->children[1]);

  list_t evaluated_args;
  create_list(&evaluated_args, malloc, free);

  for (iter_t it = list_begin(&(args->list));
       !iter_eq(it, list_end(&(args->list)));
       it = iter_next(it)) {
    plvalue_t *evaluated_arg = NEW(plvalue_t);
    *evaluated_arg =
      eval_expr((ast_node_base_t*)(iter_deref(it)), stack);
    list_push_back(&evaluated_args, evaluated_arg);
  }

  plvalue_t ret;
  if (is_builtin_call(idref->value.svalue)) {
    ret = builtin_call(idref->value.svalue, evaluated_args);
  }
  else {
    /// @todo
  }

  for (iter_t it = list_begin(&evaluated_args);
       !iter_eq(it, list_end(&evaluated_args));
       it = iter_next(it)) {
    free(iter_deref(it));
  }
  destroy_list(&evaluated_args);

  return ret;
}

plvalue_t eval_expr(ast_node_base_t *node, stack_t *stack) {
  switch (node->node_sema_info) {
  case ANS_FUNC_CALL:
    return eval_func_call((ast_dchild_t*)node, stack);
  case ANS_BINEXPR:
    return eval_binexpr((ast_dchild_wdata_t*)node, stack);
  case ANS_IDREF:
    return eval_idref_expr((ast_leaf_wdata_t*)node, stack);
  case ANS_INTVAL: case ANS_FLOATVAL: case ANS_BOOLVAL: case ANS_STR:
    return eval_literal_expr((ast_leaf_wdata_t*)node);
  }
  UNREACHABLE;
  plvalue_t failure = create_temp();
  failure.type = JT_UNDEFINED;
  return failure;
}

void eval_stmt(ast_node_base_t *stmt, stack_t *stack);

void eval_if_stmt(ast_node_base_t *stmt, stack_t *stack) {
  ast_node_base_t *cond, *thenstmt, *elsestmt = NULL;
  if (stmt->node_kind == ANK_DUAL_CHILD) {
    ast_dchild_t *ifstmt = (ast_dchild_t*)stmt;
    cond = ifstmt->children[0];
    thenstmt = ifstmt->children[1];
  }
  else {
    assert(stmt->node_kind == ANK_TRIPLE_CHILD);
    ast_tchild_t *ifstmt = (ast_tchild_t*)stmt;
    cond = ifstmt->children[0];
    thenstmt = ifstmt->children[1];
    elsestmt = ifstmt->children[2];
  }

  if (bool_failsafe(fetch_bool(eval_expr(cond, stack)))) {
    eval_stmt(thenstmt, stack);
  }
  else if (elsestmt) {
    eval_stmt(elsestmt, stack);
  }
}

void eval_while_stmt(ast_node_base_t *stmt, stack_t *stack) {
  ast_dchild_t *while_stmt = (ast_dchild_t*)stmt;
  while (bool_failsafe(fetch_bool(eval_expr(while_stmt->children[0],
          stack)))) {
    eval_stmt(while_stmt->children[1], stack);
  }
}

void eval_stmt_list(ast_node_base_t *stmt, stack_t *stack) {
  ast_list_t *stmt_list = (ast_list_t*)stmt;
  for (iter_t it = list_begin(&(stmt_list->list));
       !iter_eq(it, list_end(&(stmt_list->list)));
       it = iter_next(it)) {
    eval_stmt((ast_node_base_t*)iter_deref(it), stack);
  }
}

void eval_stmt(ast_node_base_t *stmt, stack_t *stack) {
  switch (stmt->node_sema_info) {
  case ANS_BINEXPR:
  case ANS_INTVAL:
  case ANS_FLOATVAL:
  case ANS_STR:
  case ANS_IDREF:
  case ANS_FUNC_CALL:
    eval_expr(stmt, stack);
    break;
  case ANS_IF:
    eval_if_stmt(stmt, stack);
    break;
  case ANS_WHILE:
    eval_while_stmt(stmt, stack);
    break;
  case ANS_STATEMENTS:
    eval_stmt_list(stmt, stack);
    break;
  default:
    UNREACHABLE;
  }
}

void eval_func_body(ast_list_t *body, stack_t *stack) {
  list_t stmts = body->list;
  for (iter_t it = list_begin(&stmts);
       !iter_eq(it, list_end(&stmts));
       it = iter_next(it)) {
    eval_stmt((ast_node_base_t*)iter_deref(it), stack);
  }
}

static void callfunc(ast_tchild_wdata_t *func, list_t args,
                     list_t rets, stack_t *stack) {
  stack_enter_frame(stack);
  ast_list_t *param_list_node = (ast_list_t*)func->children[0];
  list_t param_list = param_list_node->list;
  for (iter_t it1 = list_begin(&param_list),
              it2 = list_begin(&args);
       !iter_eq(it1, list_end(&param_list))
       && !iter_eq(it2, list_end(&args));
       it1 = iter_next(it1), it2 = iter_next(it2)) {
    ast_leaf_wdata_t *param_idref_node =
      (ast_leaf_wdata_t*)iter_deref(it1);
    assign(eval_idref_expr(param_idref_node, stack),
           *(plvalue_t*)iter_deref(it2));
  }

  ast_schild_t *funcbody = (ast_schild_t*)(func->children[2]);
  
  eval_func_body((ast_list_t*)(funcbody->child), stack);

  ast_list_t *rets_list_node = (ast_list_t*)func->children[1];
  list_t rets_list = rets_list_node->list;
  for (iter_t it = list_begin(&rets_list);
       !iter_eq(it, list_end(&rets_list));
       it = iter_next(it)) {
    plvalue_t *t = NEW(plvalue_t);
    *t = create_temp();
    ast_leaf_wdata_t *ret_idref_node = 
      (ast_leaf_wdata_t*)iter_deref(it);
    assign(*t, eval_idref_expr(ret_idref_node, stack));
    list_push_back(&rets, t);
  }

  stack_exit_frame(stack);
}

void eval_ast(ast_node_base_t *program) {
  stack_t stack;
  init_stack(&stack);
  init_heap();
  
  strhdl_t main_str = create_string("main");
  strhdl_t start_str = create_string("start");
  ast_list_t *functions = (ast_list_t*)program;

  list_t args, rets;
  create_list(&args, malloc, free);
  create_list(&rets, malloc, free);
  for (iter_t it = list_begin(&(functions->list));
       !iter_eq(it, list_end(&(functions->list)));
       it = iter_next(it)) {
    ast_tchild_wdata_t *func =
      (ast_tchild_wdata_t*)iter_deref(it);
    if (func->value.svalue == main_str
        || func->value.svalue == start_str) {
      callfunc(func, args, rets, &stack); 
    }
  }

  destroy_list(&args);
  destroy_list(&rets);
  
  close_heap();
  close_stack(&stack);
}
