#define TEST
#include "ast.h"
#include "ast_sema_info.h"
#include "eval.h"
#include "stack.h"
#include "vktest.h"
#include "util.h"
#include "y.tab.h"

void test_eval_idref() {
  VK_TEST_SECTION_BEGIN("evaluate idref expr")
  
  plstack_t stack;
  init_stack(&stack);
  stack_enter_frame(&stack);

  plstkobj_t *stack_a = stack_get(&stack, create_string("a"));
  stack_a->soid = SOID_INT;
  stack_a->value.ivalue = 4277;
  
  jjvalue_t t;
  t.svalue = create_string("a");
  ast_leaf_wdata_t *idref_expr = 
    (ast_leaf_wdata_t*)leaf_wdata(ANS_IDREF, t);
  
  plvalue_t result = eval_expr((ast_node_base_t*)idref_expr, &stack);
  VK_ASSERT_EQUALS(ROC_ONSTACK, result.roc);
  VK_ASSERT_EQUALS(JT_INT, result.pvt);
  VK_ASSERT_EQUALS(4277, 
                   ((plstkobj_t*)result.data.pvalue)->value.ivalue);

  stack_exit_frame(&stack);
  close_stack(&stack);
  
  VK_TEST_SECTION_END("evaluate idref expr")
}

int main() {
  VK_TEST_BEGIN
  init_heap();
  
  test_eval_idref();
  
  close_heap();
  VK_TEST_END
  return 0;
}
