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

void test_eval_literal() {
  VK_TEST_SECTION_BEGIN("evaluate literal expr")
  
  plstack_t stack;
  init_stack(&stack);
  stack_enter_frame(&stack);
  
  jjvalue_t t1, t2, t3;
  t1.ivalue = 4396;
  t2.fvalue = 7777.77;
  t3.svalue = create_string("HJYZ");
  
  ast_node_base_t *intexpr = leaf_wdata(ANS_INTVAL, t1);
  ast_node_base_t *floatexpr = leaf_wdata(ANS_FLOATVAL, t2);
  ast_node_base_t *strexpr = leaf_wdata(ANS_STR, t3);
  
  plvalue_t r1 = eval_expr(intexpr, &stack);
  plvalue_t r2 = eval_expr(floatexpr, &stack);
  plvalue_t r3 = eval_expr(strexpr, &stack);
  
  VK_ASSERT_EQUALS(ROC_TEMP, r1.roc);
  VK_ASSERT_EQUALS(ROC_TEMP, r2.roc);
  VK_ASSERT_EQUALS(ROC_TEMP, r3.roc);
  VK_ASSERT_EQUALS(JT_INT, r1.pvt);
  VK_ASSERT_EQUALS(JT_FLOAT, r2.pvt);
  VK_ASSERT_EQUALS(JT_STR, r3.pvt);
  VK_ASSERT_EQUALS(4396, r1.data.ivalue);
  VK_ASSERT_EQUALS(7777.77, r2.data.fvalue);
  VK_ASSERT_EQUALS(create_string("HJYZ"), r3.data.svalue);
  
  stack_exit_frame(&stack);
  close_stack(&stack);
  
  VK_TEST_SECTION_END("evaluate literal expr")
}

void test_eval_binary_basic() {
  VK_TEST_SECTION_BEGIN("evaluate binary expr")

  plstack_t stack;
  init_stack(&stack);
  stack_enter_frame(&stack);

  jjvalue_t t1, t2, t3, t4, t5, t6;
  t1.ivalue = 4396;
  t2.ivalue = 2900;
  t3.fvalue = 7777.0;
  t4.fvalue = 0.777;
  t5.svalue = create_string("NMSL, WSND");
  t6.svalue = create_string(", HJYZ!");

  jjvalue_t add, sub, mul, div, mod;
  add.ivalue = TK_ESYM_PLUS;
  sub.ivalue = TK_ESYM_MINUS;
  mul.ivalue = TK_ESYM_ASTER;
  div.ivalue = TK_ESYM_SLASH;
  mod.ivalue = TK_ESYM_PERCENT;

  (void)sub;
  (void)mul;
  (void)div;
  (void)mod;

  ast_node_base_t *intexpr1 = leaf_wdata(ANS_INTVAL, t1);
  ast_node_base_t *intexpr2 = leaf_wdata(ANS_INTVAL, t2);
  ast_node_base_t *floatexpr1 = leaf_wdata(ANS_FLOATVAL, t3);
  ast_node_base_t *floatexpr2 = leaf_wdata(ANS_FLOATVAL, t4);
  ast_node_base_t *strexpr1 = leaf_wdata(ANS_STR, t5);
  ast_node_base_t *strexpr2 = leaf_wdata(ANS_STR, t6);

  ast_node_base_t *intadd = 
    node2_wdata(ANS_BINEXPR, add, intexpr1, intexpr2);
  ast_node_base_t *floatadd = 
    node2_wdata(ANS_BINEXPR, add, floatexpr1, floatexpr2);
  ast_node_base_t *stradd =
    node2_wdata(ANS_BINEXPR, add, strexpr1, strexpr2);

  plvalue_t r1 = eval_expr(intadd, &stack);
  plvalue_t r2 = eval_expr(floatadd, &stack);
  plvalue_t r3 = eval_expr(stradd, &stack);

  VK_ASSERT_EQUALS(ROC_TEMP, r1.roc);
  VK_ASSERT_EQUALS(ROC_TEMP, r2.roc);
  VK_ASSERT_EQUALS(ROC_TEMP, r3.roc);
  VK_ASSERT_EQUALS(JT_INT, r1.pvt);
  VK_ASSERT_EQUALS(JT_FLOAT, r2.pvt);
  VK_ASSERT_EQUALS(JT_STR, r3.pvt);
  VK_ASSERT_EQUALS(7296, r1.data.ivalue);
  VK_ASSERT_EQUALS_F(7777.777, r2.data.fvalue);
  VK_ASSERT_EQUALS(create_string("NMSL, WSND, HJYZ!"), r3.data.svalue);

  stack_exit_frame(&stack);
  close_stack(&stack);

  VK_TEST_SECTION_END("evaluate binary expr")
}

int main() {
  VK_TEST_BEGIN
  init_heap();
  
  test_eval_idref();
  test_eval_literal();
  test_eval_binary_basic();
  
  close_heap();
  VK_TEST_END
  return 0;
}
