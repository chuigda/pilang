#ifndef EVAL_H
#define EVAL_H

#include "ast.h"

void eval_ast(ast_node_base_t *program);

#if defined(TEST) || defined (EVAL_C)

#include "stack.h"
#include "plheap.h"

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

plregobj_t create_onstack(plstkobj_t *storage);
plregobj_t create_onheap(plobj_t *storage);
plregobj_t create_inreg();

typedef enum {
  ALF_ADD, ALF_SUB, ALF_MUL, ALF_DIV, ALF_MOD
} algebraic_function_t;

plregobj_t algebraic_calc(plregobj_t lhs, plregobj_t rhs, 
                          algebraic_function_t alf);

#endif

#endif
