#include "stack.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  plstack_t stack;
  init_stack(&stack);
  stack_enter_frame(&stack, 1, 1);
  
  plstkobj_t *o1 = stack_get(&stack, create_string("a"));
  o1->soid = SOID_INT;
  o1->value.ivalue = 9;
  plstkobj_t *o2 = stack_get(&stack, create_string("b"));
  o2->soid = SOID_FLOAT;
  o2->value.fvalue = 9.0;
  
  stack_enter_frame(&stack, 1, 1);
  plstkobj_t *o3 = stack_get(&stack, create_string("b"));
  o1->soid = SOID_INT;
  o1->value.ivalue = 9;
  plstkobj_t *o4 = stack_get(&stack, create_string("a"));
  o2->soid = SOID_FLOAT;
  o2->value.fvalue = 9.0;
  
  assert(stack_get(&stack, create_string("a")) == o4);
  assert(stack_get(&stack, create_string("b")) == o3);
  
  stack_exit_frame(&stack);
  
  assert(stack_get(&stack, create_string("a")) == o1);
  assert(stack_get(&stack, create_string("b")) == o2);
  
  stack_exit_frame(&stack);
  
  free(stack.storage);
  destroy_list(&(stack.frames));
  return 0;
}
