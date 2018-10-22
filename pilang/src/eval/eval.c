#define EVAL_C
#include "eval.h"

#include "util.h"
#include "y.tab.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

plregobj_t create_onstack(plstkobj_t *storage) {
  plregobj_t ret;
  ret.roc = ROC_ONSTACK;
  switch (storage->soid) {
    case SOID_INT:   ret.pvt = PT_INT;   break;
    case SOID_FLOAT: ret.pvt = PT_FLOAT; break;
    case SOID_REF:   ret.pvt = PT_REF;   break;
    default:         ret.pvt = PT_UNDEFINED;
  }
  ret.data.pvalue = storage;
  return ret;
}

plregobj_t create_onheap(plobj_t *storage) {
  plregobj_t ret;
  ret.roc = ROC_ONHEAP;
  switch (storage->oid) {
    case OID_INT:   ret.pvt = PT_INT;   break;
    case OID_FLOAT: ret.pvt = PT_FLOAT; break;
    case OID_STR:   ret.pvt = PT_STR;   break;
    case OID_LIST:  ret.pvt = PT_LIST;  break;
    default:        ret.pvt = PT_UNDEFINED;
  }
  ret.data.pvalue = storage;
  return ret;
}

plregobj_t create_inreg() {
  plregobj_t ret;
  ret.roc = ROC_INREG;
  return ret;
}

static jjvalue_t *fetch_storage(plregobj_t *obj) {
  switch (obj->roc) {
  case ROC_INREG: return &(obj->data);
  case ROC_ONHEAP: return &(((plobj_t*)obj->data.pvalue)->value);
  case ROC_ONSTACK: {
    plstkobj_t *stkobj = (plstkobj_t*)(obj->data.pvalue);
    if (stkobj->soid == SOID_REF) {
      plobj_t *heapobj = (plobj_t*)(stkobj->value.pvalue);
      return &(heapobj->value);
    }
    return &(stkobj->value);
  }
  }
  assert(0 && "unreachable");
  return NULL;
}

static result_t fetch_int(plregobj_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return failed_result("assert ign: object does not have storage?");
  }
  
  switch (obj.pvt) {
  case PT_INT: 
    return success_result(*storage);
  case PT_FLOAT: {
    jjvalue_t shell;
    shell.fvalue = (int)(storage->ivalue);
    return success_result(shell);
  }
  case PT_STR:
    return failed_result("cannot autocast from Str to Int");
  case PT_LIST:
    return failed_result("cannot autocast from List to Int");
  case PT_REF:
    return failed_result("cannot autocast from (stack ref) to Int");
  case PT_UNDEFINED: 
    return failed_result("cannot autocast from Nothing to Int");
  }

  assert(0 && "unreachable");
}

static result_t fetch_float(plregobj_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return failed_result("assert ign: object does not have storage?");
  }
  
  switch (obj.pvt) {
  case PT_INT: {
    jjvalue_t shell;
    shell.fvalue = (double)(storage->ivalue);
    return success_result(shell);
  }
  case PT_FLOAT: return success_result(*storage);
  case PT_STR: 
    return failed_result("cannot autocast from Str to Float");
  case PT_LIST:
    return failed_result("cannot autocast from List to Float");
  case PT_REF:
    return failed_result("cannot autocast from (stack ref) to Float");
  case PT_UNDEFINED: 
    return failed_result("cannot autocast from Nothing to Float");
  }

  assert(0 && "unreachable");
}

static result_t fetch_str(plregobj_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return failed_result("assert ign: object does not have storage?");
  }

  switch (obj.pvt) {
  case PT_INT: {
    char buffer[24]; 
    sprintf(buffer, "%" PRId64, storage->ivalue);
    jjvalue_t shell;
    shell.svalue = create_string(buffer);
    return success_result(shell);
  }
  case PT_FLOAT: {
    char buffer[24]; 
    sprintf(buffer, "%lf", storage->fvalue);
    jjvalue_t shell;
    shell.svalue = create_string(buffer);
    return success_result(shell);
  }
  case PT_STR: return success_result(*storage);
  case PT_LIST: 
    return failed_result("cannot autocast fron List to Str");
  case PT_REF: 
    return failed_result("cannot autocast from (stack ref) to Str");
  }
  
  assert(0 && "unreachable");
}

static result_t fetch_list(plregobj_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return failed_result("assert ign: object does not have storage?");
  }
  
  switch (obj.pvt) {
  case PT_INT:
    return failed_result("cannot autocast from Int to List");
  case PT_FLOAT: 
    return failed_result("cannot autocast from Float to List");
  case PT_STR: 
    return failed_result("cannot autocast from Str to List");
  case PT_LIST: 
    return success_result(*storage);
  case PT_REF:
    return failed_result("cannot autocast from (stack ref) to List");
  }
  
  assert(0 && "unreachable");
}

static result_t fetch_referred(plregobj_t obj) {
  // TODO I don't know the correct semantics
  (void)obj;
  assert(0 && "not implemented");
}

static void asgn_attach_typeinfo(plregobj_t *obj, int16_t pvt) {
  obj->pvt = pvt;
  switch (obj->roc) {
    case ROC_ONHEAP: {
      plstkobj_t *stkobj = (plstkobj_t*)obj->data.pvalue;
      int16_t soid;
      switch (pvt) {
        case PT_INT:   soid = SOID_INT;   break;
        case PT_FLOAT: soid = SOID_FLOAT; break;
        case PT_STR:   soid = SOID_STR;   break;
        case PT_REF:   soid = SOID_REF;   break;
        default: assert(0 && "unreachable!");
      }
      stkobj->soid = soid;
      break;
    }
    case ROC_ONSTACK: {
      plobj_t *heapobj = (plobj_t*)obj->data.pvalue;
      int16_t oid;
      switch (pvt) {
        case PT_INT:   oid = OID_INT;   break;
        case PT_FLOAT: oid = OID_FLOAT; break;
        case PT_STR:   oid = OID_STR;   break;
        case PT_LIST:  oid = OID_LIST;  break;
        default: assert(0 && "unreachable!");
      }
      heapobj->oid = oid;
      break;
    }
    default:
    assert(0 && "unreachable!");
  }
}

static void asgn_int(plregobj_t *obj, int64_t value) {
  jjvalue_t *storage = fetch_storage(obj);
  if (storage == NULL) {
    return;
  }
  if (obj->pvt == PT_LIST) {
    destroy_list(&(storage->lsvalue));
  }
  asgn_attach_typeinfo(obj, PT_INT);
  storage->ivalue = value;
}

static void asgn_float(plregobj_t *obj, double value) {
  jjvalue_t *storage = fetch_storage(obj);
  if (storage == NULL) {
    return;
  }
  if (obj->pvt == PT_LIST) {
    destroy_list(&(storage->lsvalue));
  }
  asgn_attach_typeinfo(obj, PT_FLOAT);
  storage->fvalue = value;
}

static void asgn_str(plregobj_t *obj, int64_t value) {
  jjvalue_t *storage = fetch_storage(obj);
  if (storage == NULL) {
    return;
  }
  if (obj->pvt == PT_LIST) {
    destroy_list(&(storage->lsvalue));
  }
  asgn_attach_typeinfo(obj, PT_STR);
  storage->svalue = value;
}

static void asgn_list(plregobj_t *obj, list_t value) {
  jjvalue_t *storage = fetch_storage(obj);
  if (storage == NULL) {
    return;
  }
  if (obj->pvt == PT_LIST) {
    destroy_list(&(storage->lsvalue));
  }
  asgn_attach_typeinfo(obj, PT_LIST);
  storage->lsvalue = value;
}

static void set_undefined(plregobj_t *obj) {
  jjvalue_t *storage = fetch_storage(obj);
  if (obj->pvt == PT_LIST) {
    destroy_list(&(storage->lsvalue));
  }
  obj->pvt = PT_UNDEFINED;
}

#define EITHER_IS(VALUETYPE, LHS, RHS) \
  ((LHS).pvt == VALUETYPE || (RHS).pvt == VALUETYPE)

static int64_t int_failsafe(result_t maybe) {
  return maybe.success ? maybe.value.ivalue : 0;
}

static double float_failsafe(result_t maybe) {
  return maybe.success ? maybe.value.fvalue : 0.0;
}

static int64_t str_failsafe(result_t maybe) {
  return maybe.success ? maybe.value.svalue 
                       : create_string("undefined");
}

plregobj_t algebraic_calc(plregobj_t lhs, plregobj_t rhs,
                          algebraic_function_t alf) {
  if (alf == ALF_ADD) {
    if (EITHER_IS(PT_STR, lhs, rhs)) {
      const char* strl = get_string(str_failsafe(fetch_str(lhs)));
      const char* strr = get_string(str_failsafe(fetch_str(rhs)));
      char *temp = NEWN(char, strlen(strl) + strlen(strr) + 1);
      strcpy(temp, strl);
      strcat(temp, strr);
      int newstr = create_string(temp);
      free(temp);
    
      plregobj_t ret = create_inreg();
      ret.pvt = PT_STR;
      ret.data.svalue = newstr;
      return ret;
    }
  }
  
  if (EITHER_IS(PT_FLOAT, lhs, rhs) && alf != ALF_MOD) {
    float f1 = float_failsafe(fetch_float(lhs));
    float f2 = float_failsafe(fetch_float(rhs));
    
    plregobj_t ret = create_inreg();
    ret.pvt = PT_FLOAT;
    switch (alf) {
    case ALF_ADD: ret.data.fvalue = f1 + f2; break;
    case ALF_SUB: ret.data.fvalue = f1 - f2; break;
    case ALF_MUL: ret.data.fvalue = f1 * f2; break;
    case ALF_DIV: ret.data.fvalue = f1 / f2; break;
    default: assert(0 && "unreachable");
    }
    return ret;
  }
  else if (EITHER_IS(PT_INT, lhs, rhs)) {
    int64_t i1 = int_failsafe(fetch_int(lhs));
    int64_t i2 = int_failsafe(fetch_int(rhs));
    
    plregobj_t ret = create_inreg();
    ret.pvt = PT_INT;
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
    plregobj_t ret = create_inreg();
    ret.pvt = PT_UNDEFINED;
    return ret;
  }
}

plregobj_t assign(plregobj_t lhs, plregobj_t rhs) {
  if (lhs.roc != ROC_ONSTACK || lhs.roc != ROC_ONHEAP) {
    return lhs;
  }

  switch (rhs.pvt) {
    case PT_INT:   asgn_int(&lhs, rhs.data.ivalue);   break;
    case PT_FLOAT: asgn_float(&lhs, rhs.data.fvalue); break;
    case PT_STR:   asgn_str(&lhs, rhs.data.svalue);   break;
    case PT_LIST:  asgn_list(&lhs, rhs.data.lsvalue); break;
    case PT_REF:   assert(false && "should be handled elsewhere!");
    default:       set_undefined(&lhs);                break;
  }

  return lhs;
}

void eval_ast(ast_node_base_t *program) {
  (void)program;
}
