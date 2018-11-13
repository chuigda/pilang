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
  ALF_INVL, ALF_ADD, ALF_SUB, ALF_MUL, ALF_DIV, ALF_MOD
} algebraic_function_t;

typedef enum {
  RLF_INVL, RLF_LT, RLF_GT, RLF_EQ, RLF_NEQ, RLF_NLT, RLF_NGT
} relative_function_t;

typedef enum {
  LGF_INVL, LGF_AND, LGF_OR
} logical_function_t;

plvalue_t algebraic_calc(plvalue_t lhs, plvalue_t rhs,
                         algebraic_function_t alf);
plvalue_t relative_calc(plvalue_t lhs, plvalue_t rhs,
                        relative_function_t rlf);
plvalue_t logical_calc(ast_node_base_t *lhs, ast_node_base_t *rhs,
                       stack_t *stack, logical_function_t lgf);
plvalue_t assign(plvalue_t lhs, plvalue_t rhs);
plvalue_t builtin_call(strhdl_t name, list_t args);
plvalue_t eval_expr(ast_node_base_t *node, stack_t *stack);

#endif

#endif
