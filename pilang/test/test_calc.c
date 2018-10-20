#define TEST
#include "eval.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>

int main() {
  plstack_t test_stack;
  init_heap();
  init_stack(&test_stack);
  stack_enter_frame(&test_stack);

  plstkobj_t *stack_a = stack_get(&test_stack, create_string("a"));
  plstkobj_t *stack_b = stack_get(&test_stack, create_string("b"));

  stack_a->soid = SOID_INT;
  stack_a->value.ivalue = 1;

  stack_b->soid = SOID_INT;
  stack_b->value.ivalue = 2;

  // plobj_t *heapobj = plobj_create_int(33);

  plregobj_t sum_ab = algebraic_calc(create_onstack(stack_a),
                                     create_onstack(stack_b), ALF_ADD);
  
  fprintf(stderr, "sum_ab.roc == %d\n", sum_ab.roc);
  fprintf(stderr, "sum_ab.pvt == %d\n", sum_ab.pvt);
  fprintf(stderr, "sum_ab.data.ivalue == %" PRId64 "\n",
          sum_ab.data.ivalue);

  assert(sum_ab.roc == ROC_INREG);
  assert(sum_ab.pvt == PT_INT);
  assert(sum_ab.data.ivalue == 3);

  return 0;
}
