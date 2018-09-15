#include "eval.h"

#include "stack.h"
#include "plheap.h"
#include "util.h"
#include "y.tab.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

static plregobj_t create_inreg(plobj_t *storage) {
  plregobj_t ret;
  ret.roc = ROC_INREG;
  return ret;
}

static jjvalue_t *fetch_storage(plregobj_t *obj) {
  switch (obj->roc) {
  case ROC_INREG: return &(obj->data);
  case ROC_ONHEAP: return &(((plobj_t*)obj->data.pvalue)->value);
  case ROC_ONSTACK: return &(((plstkobj_t*)obj->data.pvalue)->value);
  case ROC_NONE: return NULL;
  }
  assert(0 && "unreachable");
  return NULL;
}

static int64_t fetch_int(plregobj_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return 0;
  }
  
  switch (obj.pvt) {
  case PT_INT: return storage->ivalue;
  case PT_FLOAT: return (int)(storage->fvalue);
  case PT_STR: return (int)strlen(get_string(storage->svalue));
  case PT_LIST: return (int)list_size(&(storage->lsvalue));
  // FIXME I don't know the correct semantics
  case PT_REF: return (int)(storage->pvalue);
  case PT_UNDEFINED: return 0;
  }

  assert(0 && "unreachable");
  return 0;
}

static double fetch_float(plregobj_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return 0.0;
  }
  
  switch (obj.pvt) {
  case PT_INT: return (double)(storage->ivalue);
  case PT_FLOAT: return storage->fvalue;
  case PT_STR: return 0.0;
  case PT_LIST: return 0.0;
  case PT_REF: return 0.0;
  case PT_UNDEFINED: return 0.0;
  }
  
  assert(0 && "unreachable");
  return 0;
}

static int64_t fetch_str(plregobj_t obj) {
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    return 0.0;
  }
  
  switch (obj.pvt) {
  case PT_INT: {
    char buffer[24]; sprintf(buffer, "%lld", storage->ivalue);
    return create_string(buffer);
  }
  case PT_FLOAT: {
    char buffer[24]; sprintf(buffer, "%lf", storage->fvalue);
    return create_string(buffer);
  }
  case PT_STR: return storage->svalue;
  case PT_LIST: return create_string("");
  case PT_REF: return create_string("");
  }
  
  assert(0 && "unreachable");
  return create_string("");
}

static list_t fetch_list(plregobj_t obj) {
  void* (*m)(size_t) = malloc;
  void (*f)(void*) = free;
  
  jjvalue_t *storage = fetch_storage(&obj);
  if (storage == NULL) {
    list_t list;
    create_list(&list, m, f);
    return list;
  }
  
  switch (obj.pvt) {
  case PT_INT: { list_t list; create_list(&list, m, f); return list; }
  case PT_FLOAT: { list_t list; create_list(&list, m, f); return list; }
  case PT_STR: { list_t list; create_list(&list, m, f); return list; }
  case PT_LIST: return storage->lsvalue;
  case PT_REF: { list_t list; create_list(&list, m, f); return list; }
  }
  
  assert(0 && "unreachable");
  list_t list;
  create_list(&list, m, f);
  return list;
}

static plobj_t *fetch_referred(plregobj_t obj) {
  // TODO I don't know the correct semantics
  assert(0 && "not implemented");
  return NULL;
}

void eval_ast(ast_node_base_t *program) {
  (void)program;
}
