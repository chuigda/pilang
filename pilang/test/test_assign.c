#define TEST
#include "eval.h"
#include "stack.h"
#include "vktest.h"
#include "util.h"

void test_stack_assign() {
  VK_TEST_SECTION_BEGIN("assign to stack variable")

  plstack_t stack;
  init_stack(&stack);
  stack_enter_frame(&stack);

  plstkobj_t *stack_a = stack_get(&stack, create_string("a"));
  plvalue_t value = create_temp();
  value.pvt = JT_INT;
  value.data.ivalue = 7777;

  VK_ASSERT_EQUALS(ROC_ONSTACK, create_onstack(stack_a).roc);

  assign(create_onstack(stack_a), value);

  VK_ASSERT_EQUALS(JT_INT,   create_onstack(stack_a).pvt);
  VK_ASSERT_EQUALS(SOID_INT, stack_a->soid);
  VK_ASSERT_EQUALS(7777,     stack_a->value.ivalue);

  stack_exit_frame(&stack);
  close_stack(&stack);

  VK_TEST_SECTION_END("assign to stack variable")
}

void test_heap_assign() {
  VK_TEST_SECTION_BEGIN("assign to heap object")

  plheapobj_t *heapobj = plobj_create_int(99);
  plvalue_t value = create_temp();
  value.pvt = JT_INT;
  value.data.ivalue = 7777;

  VK_ASSERT_EQUALS(JT_INT, create_onheap(heapobj).pvt);
  VK_ASSERT_EQUALS(99, heapobj->value.ivalue);

  assign(create_onheap(heapobj), value);

  VK_ASSERT_EQUALS(7777, heapobj->value.ivalue);

  VK_TEST_SECTION_END("assign to heap object")
}

void test_assignto_stackref() {
  VK_TEST_SECTION_BEGIN("assign to stack reference")
  
  plstack_t stack;
  init_stack(&stack);
  stack_enter_frame(&stack);

  plheapobj_t *heapobj = plobj_create_int(2900);
  
  plstkobj_t *ref = stack_get(&stack, create_string("ref1"));
  ref->soid = SOID_REF;
  ref->value.pvalue = heapobj;
  
  plvalue_t value = create_temp();
  value.pvt = JT_INT;
  value.data.ivalue = 4396;
  
  assign(create_onstack(ref), value);
  
  VK_ASSERT_EQUALS(HOID_INT, heapobj->oid);
  VK_ASSERT_EQUALS(4396, heapobj->value.ivalue);
  VK_ASSERT_EQUALS(SOID_REF, ref->soid);
  VK_ASSERT_EQUALS(heapobj, ref->value.pvalue);

  stack_exit_frame(&stack);
  close_stack(&stack);

  VK_TEST_SECTION_END("assign to stack reference")
}

int main() {
  VK_TEST_BEGIN
  init_heap();

  test_stack_assign();
  test_heap_assign();
  test_assignto_stackref();

  close_heap();
  VK_TEST_END
}
