#include "eval.h"

#include "stack.h"
#include "plheap.h"
#include "util.h"
#include "y.tab.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef enum {
  ROC_INREG,
  ROC_ONSTACK,
  ROC_ONHEAP,
  ROC_NONE
} plregobj_cont_t;

typedef enum {
  PT_INT,
  PT_FLOAT,
  PT_STR,
  PT_LIST,
  PT_REF,
  PT_UNDEFINED
} pl_value_type_t;

typedef struct {
  jjvalue_t data;
  int16_t roc;
  int16_t pvt;
} plregobj_t;

static plregobj_t create_onstack(plstkobj_t *storage) {
  plregobj_t ret;
  ret.roc = ROC_ONSTACK;
  ret.data.pvalue = storage;
  return ret;
}

static plregobj_t create_onheap(plobj_t *storage) {
  plregobj_t ret;
  ret.roc = ROC_ONHEAP;
  ret.data.pvalue = storage;
  return ret;
}

static plregobj_t create_inreg() {
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

static void putin_int(plregobj_t *obj, int64_t value) {
  jjvalue_t *storage = fetch_storage(obj);
  if (storage == NULL) {
    return;
  }
  if (obj->pvt == PT_LIST) {
    destroy_list(&(storage->lsvalue));
  }
  obj->pvt = PT_INT;
  storage->ivalue = value;
}

static void putin_float(plregobj_t *obj, double value) {
  jjvalue_t *storage = fetch_storage(obj);
  if (storage == NULL) {
    return;
  }
  if (obj->pvt == PT_LIST) {
    destroy_list(&(storage->lsvalue));
  }
  obj->pvt = PT_FLOAT;
  storage->fvalue = value;
}

static void putin_str(plregobj_t *obj, int64_t value) {
  jjvalue_t *storage = fetch_storage(obj);
  if (storage == NULL) {
    return;
  }
  if (obj->pvt == PT_LIST) {
    destroy_list(&(storage->lsvalue));
  }
  obj->pvt = PT_STR;
  storage->svalue = value;
}

static void putin_list(plregobj_t *obj, list_t value) {
  jjvalue_t *storage = fetch_storage(obj);
  if (storage == NULL) {
    return;
  }
  if (obj->pvt == PT_LIST) {
    destroy_list(&(storage->lsvalue));
  }
  obj->pvt = PT_LIST;
  storage->lsvalue = value;
}

void eval_ast(ast_node_base_t *program) {
  (void)program;
}
