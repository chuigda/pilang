#define EVAL_C
#include "eval.h"

#include "ast.h"
#include "util.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

plvalue_t create_onstack(plstkobj_t *storage) {
  plvalue_t ret;
  ret.roc = ROC_ONSTACK;
  ret.pvt = soid2jt(storage->soid);
  ret.data.pvalue = storage;
  return ret;
}

plvalue_t create_onheap(plheapobj_t *storage) {
  plvalue_t ret;
  ret.roc = ROC_ONHEAP;
  ret.pvt = hoid2jt(storage->oid);
  ret.data.pvalue = storage;
  return ret;
}

plvalue_t create_inreg() {
  plvalue_t ret;
  ret.roc = ROC_TEMP;
  return ret;
}

static jjvalue_t *fetch_storage(plvalue_t *obj) {
  switch (obj->roc) {
  case ROC_TEMP: return &(obj->data);
  case ROC_ONHEAP: return &(((plheapobj_t*)obj->data.pvalue)->value);
  case ROC_ONSTACK: {
    plstkobj_t *stkobj = (plstkobj_t*)(obj->data.pvalue);
    if (stkobj->soid == SOID_REF) {
      plheapobj_t *heapobj = (plheapobj_t*)(stkobj->value.pvalue);
      return &(heapobj->value);
    }
    return &(stkobj->value);
  }
  }
  UNREAECHABLE
  return NULL;
}

static result_t fetch_int(plvalue_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return failed_result("assert ign: object does not have storage?");
  }
  
  switch (obj.pvt) {
  case JT_INT: 
    return success_result(*storage);
  case JT_FLOAT: {
    jjvalue_t shell;
    shell.fvalue = (int)(storage->ivalue);
    return success_result(shell);
  }
  case JT_STR:
    return failed_result("cannot autocast from Str to Int");
  case JT_LIST:
    return failed_result("cannot autocast from List to Int");
  case JT_UNDEFINED: 
    return failed_result("cannot autocast from Nothing to Int");
  }

  assert(0 && "unreachable");
}

static result_t fetch_float(plvalue_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return failed_result("assert ign: object does not have storage?");
  }
  
  switch (obj.pvt) {
  case JT_INT: {
    jjvalue_t shell;
    shell.fvalue = (double)(storage->ivalue);
    return success_result(shell);
  }
  case JT_FLOAT: return success_result(*storage);
  case JT_STR: 
    return failed_result("cannot autocast from Str to Float");
  case JT_LIST:
    return failed_result("cannot autocast from List to Float");
  case JT_UNDEFINED: 
    return failed_result("cannot autocast from Nothing to Float");
  }

  UNREAECHABLE
}

static result_t fetch_str(plvalue_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return failed_result("assert ign: object does not have storage?");
  }

  switch (obj.pvt) {
  case JT_INT: {
    char buffer[24]; 
    sprintf(buffer, "%" PRId64, storage->ivalue);
    jjvalue_t shell;
    shell.svalue = create_string(buffer);
    return success_result(shell);
  }
  case JT_FLOAT: {
    char buffer[24]; 
    sprintf(buffer, "%lf", storage->fvalue);
    jjvalue_t shell;
    shell.svalue = create_string(buffer);
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
    return failed_result("assert ign: object does not have storage?");
  }
  
  switch (obj.pvt) {
  case JT_INT:
    return failed_result("cannot autocast from Int to List");
  case JT_FLOAT: 
    return failed_result("cannot autocast from Float to List");
  case JT_STR: 
    return failed_result("cannot autocast from Str to List");
  case JT_LIST: 
    return success_result(*storage);
  }
  
  UNREAECHABLE
}

static void asgn_attach_typeinfo(plvalue_t *obj, int16_t pvt) {
  obj->pvt = pvt;
  switch (obj->roc) {
    case ROC_ONHEAP: {
      plstkobj_t *stkobj = (plstkobj_t*)obj->data.pvalue;
      stkobj->soid = jt2soid(pvt);
      break;
    }
    case ROC_ONSTACK: {
      plheapobj_t *heapobj = (plheapobj_t*)obj->data.pvalue;
      heapobj->oid = jt2hoid(pvt);
      break;
    }
    default:
    UNREAECHABLE
  }
}

static void asgn_int(plvalue_t *obj, int64_t value) {
  jjvalue_t *storage = fetch_storage(obj);
  if (storage == NULL) {
    return;
  }
  if (obj->roc == ROC_ONHEAP) {
    destroy_object((plheapobj_t*)(obj->data.pvalue));
  }
  asgn_attach_typeinfo(obj, JT_INT);
  storage->ivalue = value;
}

static void asgn_float(plvalue_t *obj, double value) {
  jjvalue_t *storage = fetch_storage(obj);
  if (storage == NULL) {
    return;
  }
  if (obj->roc == ROC_ONHEAP) {
    destroy_object((plheapobj_t*)(obj->data.pvalue));
  }
  asgn_attach_typeinfo(obj, JT_FLOAT);
  storage->fvalue = value;
}

static void asgn_str(plvalue_t *obj, int64_t value) {
  jjvalue_t *storage = fetch_storage(obj);
  if (storage == NULL) {
    return;
  }
  if (obj->roc == ROC_ONHEAP) {
    destroy_object((plheapobj_t*)(obj->data.pvalue));
  }
  asgn_attach_typeinfo(obj, JT_STR);
  storage->svalue = value;
}

static void asgn_list(plvalue_t *obj, list_t value) {
  jjvalue_t *storage = fetch_storage(obj);
  if (storage == NULL) {
    return;
  }
  if (obj->roc == ROC_ONHEAP) {
    destroy_object((plheapobj_t*)(obj->data.pvalue));
  }
  asgn_attach_typeinfo(obj, JT_LIST);
  storage->lsvalue = value;
}

static void asgn_ref(plvalue_t *obj, void *value) {
  jjvalue_t *storage = fetch_storage(obj);
  if (storage == NULL) {
    return;
  }
  if (obj->roc == ROC_ONHEAP) {
    destroy_object((plheapobj_t*)(obj->data.pvalue));
  }
  asgn_attach_typeinfo(obj, JT_REF);
  storage->pvalue = value;
}

static void set_undefined(plvalue_t *obj) {
  if (obj->roc == ROC_ONHEAP) {
    destroy_object((plheapobj_t*)(obj->data.pvalue));
  }
  obj->pvt = JT_UNDEFINED;
}

#define EITHER_IS(VALUETYPE, LHS, RHS) \
  ((LHS).pvt == VALUETYPE || (RHS).pvt == VALUETYPE)

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

plvalue_t algebraic_calc(plvalue_t lhs, plvalue_t rhs,
                         algebraic_function_t alf) {
  if (alf == ALF_ADD) {
    if (EITHER_IS(JT_STR, lhs, rhs)) {
      const char* strl = get_string(str_failsafe(fetch_str(lhs)));
      const char* strr = get_string(str_failsafe(fetch_str(rhs)));
      char *temp = NEWN(char, strlen(strl) + strlen(strr) + 1);
      strcpy(temp, strl);
      strcat(temp, strr);
      int newstr = create_string(temp);
      free(temp);
    
      plvalue_t ret = create_inreg();
      ret.pvt = JT_STR;
      ret.data.svalue = newstr;
      return ret;
    }
  }
  
  if (EITHER_IS(JT_FLOAT, lhs, rhs) && alf != ALF_MOD) {
    float f1 = float_failsafe(fetch_float(lhs));
    float f2 = float_failsafe(fetch_float(rhs));
    
    plvalue_t ret = create_inreg();
    ret.pvt = JT_FLOAT;
    switch (alf) {
    case ALF_ADD: ret.data.fvalue = f1 + f2; break;
    case ALF_SUB: ret.data.fvalue = f1 - f2; break;
    case ALF_MUL: ret.data.fvalue = f1 * f2; break;
    case ALF_DIV: ret.data.fvalue = f1 / f2; break;
    default: UNREAECHABLE
    }
    return ret;
  }
  else if (EITHER_IS(JT_INT, lhs, rhs)) {
    int64_t i1 = int_failsafe(fetch_int(lhs));
    int64_t i2 = int_failsafe(fetch_int(rhs));
    
    plvalue_t ret = create_inreg();
    ret.pvt = JT_INT;
    switch (alf) {
    case ALF_ADD: ret.data.ivalue = i1 + i2; break;
    case ALF_SUB: ret.data.ivalue = i1 - i2; break;
    case ALF_MUL: ret.data.ivalue = i1 * i2; break;
    case ALF_DIV: ret.data.ivalue = i1 / i2; break;
    case ALF_MOD: ret.data.ivalue = i1 % i2; break;
    }
    return ret;
  }
  else {
    plvalue_t ret = create_inreg();
    ret.pvt = JT_UNDEFINED;
    return ret;
  }
}

plvalue_t assign(plvalue_t lhs, plvalue_t rhs) {
  if (lhs.roc != ROC_ONSTACK || lhs.roc != ROC_ONHEAP) {
    return lhs;
  }

  switch (rhs.pvt) {
    case JT_INT:   asgn_int(&lhs, rhs.data.ivalue);    break;
    case JT_FLOAT: asgn_float(&lhs, rhs.data.fvalue);  break;
    case JT_STR:   asgn_str(&lhs, rhs.data.svalue);    break;
    case JT_LIST:  asgn_list(&lhs, rhs.data.lsvalue);  break;
    case JT_REF:   asgn_ref(&lhs, rhs.data.pvalue);    break;
    default:       set_undefined(&lhs);                break;
  }

  return lhs;
}

plvalue_t eval_literal_expr(ast_leaf_wdata_t *node) {
  plvalue_t ret = create_inreg();
  switch (node->node_sema_info) {
    case ANS_INTVAL:   ret.pvt = JT_INT;   break;
    case ANS_FLOATVAL: ret.pvt = JT_FLOAT; break;
    case ANS_STR:      ret.pvt = JT_STR;   break;
  }
  ret.data = node->data;
  return ret;
}

plvalue_t eval_idref_expr(ast_leaf_wdata_t *node, plstack_t *stack) {
  return create_onstack(stack_get(stack, node->data.svalue));
}

void eval_stmt(ast_node_base_t *stmt, plstack_t *stack) {
  // TODO
  (void)stmt;
  (void)stack;
}

void eval_func_body(ast_list_t *body, plstack_t *stack) {
  list_t stmts = body->list;
  for (iter_t it = list_begin(&stmts); 
       !iter_eq(it, list_end(&stmts));
       it = iter_next(it)) {
    eval_stmt((ast_node_base_t*)iter_deref(it), stack);
  }
}

static void callfunc(ast_tchild_wdata_t *func, list_t args, 
                     list_t rets, plstack_t *stack) {
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
    *t = create_inreg();
    ast_leaf_wdata_t *ret_idref_node = (ast_leaf_wdata_t*)iter_deref(it);
    assign(*t, eval_idref_expr(ret_idref_node, stack));
    list_push_back(&rets, t);
  }

  stack_exit_frame(stack);
}

void eval_ast(ast_node_base_t *program) {
  (void)program;
}
