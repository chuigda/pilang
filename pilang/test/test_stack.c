#include "stack.h"

#include "vktest.h"

int main() {
  VK_TEST_BEGIN

  plstack_t stack;
  init_stack(&stack);
  stack_enter_frame(&stack);
  
  int64_t name_a = create_string("a");
  int64_t name_b = create_string("b");
  
  plstkobj_t *o1 = stack_get(&stack, name_a);
  o1->soid = SOID_INT;
  o1->value.ivalue = 9;
  plstkobj_t *o2 = stack_get(&stack, name_b);
  o2->soid = SOID_FLOAT;
  o2->value.fvalue = 9.0;
  
  stack_enter_frame(&stack);
  plstkobj_t *o3 = stack_get(&stack, name_b);
  o1->soid = SOID_INT;
  o1->value.ivalue = 9;
  plstkobj_t *o4 = stack_get(&stack, name_a);
  o2->soid = SOID_FLOAT;
  o2->value.fvalue = 9.0;
  
  VK_ASSERT_EQUALS(o4, stack_get(&stack, name_a));
  VK_ASSERT_EQUALS(o3, stack_get(&stack, name_b));
  
  VK_ASSERT_NOT_EQUALS(o4, o1);
  VK_ASSERT_NOT_EQUALS(o3, o2);
  
  stack_exit_frame(&stack);
  
  VK_ASSERT_EQUALS(o1, stack_get(&stack, name_a));
  VK_ASSERT_EQUALS(o2, stack_get(&stack, name_b));
  
  stack_exit_frame(&stack);
  
  free(stack.storage);
  destroy_list(&(stack.frames));

  VK_TEST_END
  return 0;
}
