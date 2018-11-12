#ifndef EVAL_H
#define EVAL_H

#include "ast.h"

void eval_ast(ast_node_base_t *program);

#if defined(TEST) || defined (EVAL_C)

#include "stack.h"
#include "heap.h"

typedef enum {
  ROC_TEMP,
  ROC_ONSTACK,
  ROC_ONHEAP,
  ROC_NONE
} plvalue_storage_t;

typedef struct {
  jjvalue_t value;
  int16_t roc;
  int16_t type;
} plvalue_t;

plvalue_t create_onstack(stkobj_t *storage);
plvalue_t create_onheap(heapobj_t *storage);
plvalue_t create_temp();
jjvalue_t *fetch_storage(plvalue_t *obj);

typedef enum {
  ALF_ADD, ALF_SUB, ALF_MUL, ALF_DIV, ALF_MOD
} algebraic_function_t;

plvalue_t algebraic_calc(plvalue_t lhs, plvalue_t rhs,
                         algebraic_function_t alf);
plvalue_t assign(plvalue_t lhs, plvalue_t rhs);
plvalue_t eval_expr(ast_node_base_t *node, stack_t *stack);

#endif

#endif
