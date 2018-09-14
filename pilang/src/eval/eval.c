#include "eval.h"

#include "stack.h"
#include "plheap.h"
#include "util.h"
#include "y.tab.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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

static plregobj_t maybe_fetch(plregobj_t maybe_onstack) {
  if (maybe_onstack.roc != ROC_ONSTACK) {
    return maybe_onstack;
  }
  
  plregobj_t ret;
  ret.roc = INREG;
  ret.pvt = maybe_onstack.pvt;
  plstkobj_t *stkobj = (plstkobj_t*)maybe_onstack.data.pvalue;
  ret.data = stkobj->data;
  return ret;
}

void eval_ast(ast_node_base_t *program) {
  (void)program;
}
