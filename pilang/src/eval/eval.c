#define EVAL_C
#include "eval.h"

#include "ast.h"
#include "util.h"
#include "y.tab.h"

#include <assert.h>
#include <float.h>
#include <inttypes.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

plvalue_t create_onstack(stkobj_t *storage) {
  plvalue_t ret;
  ret.roc = ROC_ONSTACK;
  ret.type = soid2jt(storage->soid);
  ret.value.pvalue = storage;
  return ret;
}

plvalue_t create_onheap(heapobj_t *storage) {
  plvalue_t ret;
  ret.roc = ROC_ONHEAP;
  ret.type = hoid2jt(storage->oid);
  ret.value.pvalue = storage;
  return ret;
}

plvalue_t create_temp() {
  plvalue_t ret;
  ret.roc = ROC_TEMP;
  return ret;
}

jjvalue_t *fetch_storage(plvalue_t *obj) {
  switch (obj->roc) {
  case ROC_TEMP: return &(obj->value);
  case ROC_ONHEAP: {
    heapobj_t *heapobj = (heapobj_t*)(obj->value.pvalue);
    return &(heapobj->value);
  }
  case ROC_ONSTACK: {
    stkobj_t *stkobj = (stkobj_t*)(obj->value.pvalue);
    return &(stkobj->value);
  }
  }
  return NULL;
}

static result_t fetch_int(plvalue_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return failed_result("object does not have storage");
  }

  switch (obj.type) {
  case JT_INT:
    return success_result(*storage);
  case JT_FLOAT: {
    jjvalue_t shell;
    shell.fvalue = (int)(storage->ivalue);
    return success_result(shell);
  }
  case JT_BOOL: {
    jjvalue_t shell;
    shell.ivalue = (int)(storage->bvalue);
    return success_result(shell);
  }
  case JT_STR:
    return failed_result("cannot autocast from Str to Int");
  case JT_LIST:
    return failed_result("cannot autocast from List to Int");
  case JT_UNDEFINED:
    return failed_result("cannot autocast from Nothing to Int");
  }

  UNREAECHABLE
}

static result_t fetch_float(plvalue_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return failed_result("object does not have storage");
  }

  switch (obj.type) {
  case JT_INT: {
    jjvalue_t shell;
    shell.fvalue = (double)(storage->ivalue);
    return success_result(shell);
  }
  case JT_FLOAT: return success_result(*storage);
  case JT_BOOL: {
    jjvalue_t shell;
    shell.fvalue = (float)(storage->fvalue);
    return success_result(shell);
  }
  case JT_STR:
    return failed_result("cannot autocast from Str to Float");
  case JT_LIST:
    return failed_result("cannot autocast from List to Float");
  case JT_UNDEFINED:
    return failed_result("cannot autocast from Nothing to Float");
  }

  UNREAECHABLE
}

static result_t fetch_bool(plvalue_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return failed_result("object does not have storage");
  }

  switch (obj.type) {
  case JT_INT: {
    jjvalue_t shell;
    shell.bvalue = (bool)(storage->ivalue);
    return success_result(shell);
  }
  case JT_FLOAT: {
    jjvalue_t shell;
    shell.bvalue = (int)(storage->fvalue);
    return success_result(shell);
  }
  case JT_BOOL: {
    jjvalue_t shell;
    shell.bvalue = storage->bvalue;
    return success_result(shell);
  }
  case JT_STR:
  case JT_LIST: {
    jjvalue_t shell;
    shell.bvalue = true;
    return success_result(shell);
  }
  case JT_UNDEFINED: {
    jjvalue_t shell;
    shell.bvalue = false;
    return success_result(shell);
  }
  }

  UNREAECHABLE
}

static result_t fetch_str(plvalue_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return failed_result("object does not have storage");
  }

  switch (obj.type) {
  case JT_INT: {
    char buffer[128];
    sprintf(buffer, "%" PRId64, storage->ivalue);
    jjvalue_t shell;
    shell.svalue = create_string(buffer);
    return success_result(shell);
  }
  case JT_FLOAT: {
    char buffer[128];
    sprintf(buffer, "%g", storage->fvalue);
    jjvalue_t shell;
    shell.svalue = create_string(buffer);
    return success_result(shell);
  }
  case JT_BOOL: {
    jjvalue_t shell;
    shell.svalue = storage->bvalue ? create_string("True") :
                                     create_string("False");
    return success_result(shell);
  }
  case JT_STR: return success_result(*storage);
  case JT_LIST:
    return failed_result("cannot autocast fron List to Str");
  }

  UNREAECHABLE
}

static result_t fetch_list(plvalue_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return failed_result("object does not have storage");
  }

  switch (obj.type) {
  case JT_INT:
    return failed_result("cannot autocast from Int to List");
  case JT_FLOAT:
    return failed_result("cannot autocast from Float to List");
  case JT_BOOL:
    return failed_result("cannot autocast from Bool to List");
  case JT_STR:
    return failed_result("cannot autocast from Str to List");
  case JT_LIST:
    return success_result(*storage);
  }

  UNREAECHABLE
}

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
    UNREAECHABLE
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

static plvalue_t auto_deref(plvalue_t maybe_ref) {
  if (maybe_ref.type != JT_REF) {
    return maybe_ref;
  }
  heapobj_t *referred =
    (heapobj_t*)(fetch_storage(&maybe_ref)->pvalue);
  return create_onheap(referred);
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
    default: UNREAECHABLE
    }
    return ret;
  }
  else if (EITHER_IS(JT_INT, lhs, rhs) || EITHER_IS(JT_BOOL, lhs, rhs)) {
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
    default: UNREAECHABLE
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
    default: UNREAECHABLE
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
    default: UNREAECHABLE
    }
  }
  else if (EITHER_IS(JT_INT, lhs, rhs) || EITHER_IS(JT_BOOL, lhs, rhs)) {
    int i1 = int_failsafe(fetch_int(lhs));
    int i2 = int_failsafe(fetch_int(rhs));
    switch (rlf) {
    case RLF_EQ:  ret.value.ivalue = i1 == i2; break;
    case RLF_NEQ: ret.value.ivalue = i1 != i2; break;
    case RLF_LT:  ret.value.bvalue = i1 < i2;  break;
    case RLF_GT:  ret.value.bvalue = i1 > i2;  break;
    case RLF_NLT: ret.value.bvalue = i1 >= i2; break;
    case RLF_NGT: ret.value.bvalue = i1 <= i2; break;
    default: UNREAECHABLE
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
  default: UNREAECHABLE
  }

  return ret;
}

plvalue_t assign(plvalue_t lhs, plvalue_t rhs) {
  if (lhs.roc != ROC_ONSTACK && lhs.roc != ROC_ONHEAP) {
    return lhs;
  }

  if (lhs.roc == ROC_ONSTACK) {
    lhs = auto_deref(lhs);
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
  return lhs;
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
      UNREAECHABLE
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

typedef plvalue_t (*builtin_func_t)(list_t);

static plvalue_t builtin_print(list_t args) {
  for (iter_t it = list_begin(&args);
       !iter_eq(it, list_end(&args));
       it = iter_next(it)) {
    plvalue_t *value = (plvalue_t*)iter_deref(it);
    *value = auto_deref(*value);
    jjvalue_t *storage = fetch_storage(value);

    switch (value->type) {
    case JT_INT:
      printf("%" PRId64, storage->ivalue); break;
    case JT_FLOAT:
      printf("%f", storage->fvalue); break;
    case JT_BOOL:
      printf("%s", storage->bvalue ? "true" : "false");
      break;
    case JT_STR:
      printf("%s", get_string(storage->svalue)); break;
    case JT_LIST:
      printf("(list)"); break;
    case JT_UNDEFINED:
      printf("Undefined"); break;
    default:
      printf("error");
    }
  }
  plvalue_t ret = create_temp();
  ret.type = JT_INT;
  ret.value.ivalue = list_size(&args);
  return ret;
}

static plvalue_t builtin_readint(list_t args) {
  (void)args;
  plvalue_t ret = create_temp();
  ret.type = JT_INT;
  scanf("%" PRId64, &(ret.value.ivalue));
  return ret;
}

static plvalue_t builtin_readfloat(list_t args) {
  (void)args;
  plvalue_t ret = create_temp();
  ret.type = JT_FLOAT;
  scanf("%lf", &(ret.value.fvalue));
  return ret;
}

static plvalue_t builtin_readstr(list_t args) {
  (void)args;
  static char buffer[4096];
  plvalue_t ret = create_temp();
  ret.type = JT_STR;
  scanf("%s", buffer);
  ret.value.svalue = create_string(buffer);
  return ret;
}

static plvalue_t builtin_copy_to_heap(list_t args) {
  if (list_size(&args) < 1) {
    eprintf0("e: toheap requires one argument\n");
    plvalue_t ret = create_temp();
    ret.type = JT_UNDEFINED;
    return ret;
  }

  if (list_size(&args) > 2) {
    eprintf0("w: toheap require only one argument\n");
  }

  plvalue_t *value = (plvalue_t*)iter_deref(list_begin(&args));
  *value = auto_deref(*value);
  jjvalue_t *storage = fetch_storage(value);

  if (value->roc == ROC_ONHEAP) {
    eprintf0("e: already on heap\n");
    plvalue_t ret = create_temp();
    ret.type = JT_UNDEFINED;
    return ret;
  }

  plvalue_t ref = create_temp();
  ref.type = JT_REF;
  switch (value->type) {
  case JT_INT:
    ref.value.pvalue = heap_alloc_int(storage->ivalue); break;
  case JT_FLOAT:
    ref.value.pvalue = heap_alloc_float(storage->fvalue); break;
  case JT_STR:
    ref.value.pvalue = heap_alloc_str(storage->svalue); break;
  case JT_BOOL:
    ref.value.pvalue = heap_alloc_bool(storage->bvalue); break;
  case JT_UNDEFINED:
    ref.value.pvalue = heap_alloc_empty(); break;
  default: UNREAECHABLE
  }
  return ref;
}

plvalue_t builtin_call(strhdl_t name, list_t args) {
  /// @todo replace this with TableGen
  static bool initialized = false;
  static strhdl_t builtin_func_names[11];
  if (!initialized) {
    builtin_func_names[0] = create_string("print");
    builtin_func_names[1] = create_string("readint");
    builtin_func_names[2] = create_string("readfloat");
    builtin_func_names[3] = create_string("readstr");
    builtin_func_names[4] = create_string("toheap");
    builtin_func_names[5] = create_string("fromheap");
    builtin_func_names[6] = create_string("createlist");
    builtin_func_names[7] = create_string("pushback");
    builtin_func_names[8] = create_string("popback");
    builtin_func_names[9] = create_string("atput");
    builtin_func_names[10] = create_string("at");
    initialized = true;
  }

  static builtin_func_t builtin_funcs[11] = {
    builtin_print, builtin_readint, builtin_readfloat, builtin_readstr,
    builtin_copy_to_heap, NULL, NULL, NULL, NULL, NULL, NULL
  };

  plvalue_t ret = create_temp();
  ret.type = JT_UNDEFINED;

  for (size_t i = 0; i < COUNTOF(builtin_func_names); i++) {
    if (name == builtin_func_names[i]) {
      if (builtin_funcs[i] == NULL) {
        eprintf("sorry, %s not implemented yet.\n", get_string(name));
        break;
      }
      ret = (builtin_funcs[i])(args);
    }
  }

  return ret;
}

plvalue_t eval_func_call(ast_dchild_t *func, stack_t *stack) {
  /// @todo now builtin functions only. Add user-defined functions afte
  /// releasing version Perseus
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

  plvalue_t ret = builtin_call(idref->value.svalue, evaluated_args);

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
  UNREAECHABLE;
  plvalue_t failure = create_temp();
  failure.type = JT_UNDEFINED;
  return failure;
}

void eval_stmt(ast_node_base_t *stmt, stack_t *stack) {
  // TODO
  (void)stmt;
  (void)stack;
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

  eval_func_body((ast_list_t*)(func->children[2]), stack);

  ast_list_t *rets_list_node = (ast_list_t*)func->children[1];
  list_t rets_list = rets_list_node->list;
  for (iter_t it = list_begin(&rets_list);
       !iter_eq(it, list_end(&rets_list));
       it = iter_next(it)) {
    plvalue_t *t = NEW(plvalue_t);
    *t = create_temp();
    ast_leaf_wdata_t *ret_idref_node = (ast_leaf_wdata_t*)iter_deref(it);
    assign(*t, eval_idref_expr(ret_idref_node, stack));
    list_push_back(&rets, t);
  }

  stack_exit_frame(stack);
}

void eval_ast(ast_node_base_t *program) {
  (void)program;
}
