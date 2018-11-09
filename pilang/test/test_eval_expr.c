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

  jjvalue_t add, sub, mul;
  add.ivalue = TK_ESYM_PLUS;
  sub.ivalue = TK_ESYM_MINUS;
  mul.ivalue = TK_ESYM_ASTER;

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

  ast_node_base_t *intsub = 
    node2_wdata(ANS_BINEXPR, sub, intexpr1, intexpr2);
  ast_node_base_t *floatsub =
    node2_wdata(ANS_BINEXPR, sub, floatexpr1, floatexpr2);

  plvalue_t r4 = eval_expr(intsub, &stack);
  plvalue_t r5 = eval_expr(floatsub, &stack);
  
  VK_ASSERT_EQUALS(ROC_TEMP, r4.roc);
  VK_ASSERT_EQUALS(ROC_TEMP, r5.roc);
  VK_ASSERT_EQUALS(JT_INT, r4.pvt);
  VK_ASSERT_EQUALS(JT_FLOAT, r5.pvt);
  VK_ASSERT_EQUALS(4396 - 2900, r4.data.ivalue);
  VK_ASSERT_EQUALS_F(7777.0 - 0.777, r5.data.fvalue);
  
  ast_node_base_t *intmul =
    node2_wdata(ANS_BINEXPR, mul, intexpr1, intexpr2);
  ast_node_base_t *floatmul =
    node2_wdata(ANS_BINEXPR, mul, floatexpr1, floatexpr2);

  plvalue_t r6 = eval_expr(intmul, &stack);
  plvalue_t r7 = eval_expr(floatmul, &stack);

  VK_ASSERT_EQUALS(ROC_TEMP, r6.roc);
  VK_ASSERT_EQUALS(ROC_TEMP, r7.roc);
  VK_ASSERT_EQUALS(JT_INT, r6.pvt);
  VK_ASSERT_EQUALS(JT_FLOAT, r7.pvt);
  VK_ASSERT_EQUALS(4396 * 2900, r6.data.ivalue);
  VK_ASSERT_EQUALS_F(7777 * 0.777, r7.data.fvalue);

  stack_exit_frame(&stack);
  close_stack(&stack);

  VK_TEST_SECTION_END("evaluate binary expr")
}

void test_eval_coercion() {
  VK_TEST_SECTION_BEGIN("evaluate with coercion")
  
  plstack_t stack;
  init_stack(&stack);
  stack_enter_frame(&stack);

  jjvalue_t t1, t3, t4, t6;
  t1.ivalue = 4396;
  t3.fvalue = 7777.0;
  t4.fvalue = 0.777;
  t6.svalue = create_string(", HJYZ!");

  jjvalue_t add, sub, mul;
  add.ivalue = TK_ESYM_PLUS;
  sub.ivalue = TK_ESYM_MINUS;
  mul.ivalue = TK_ESYM_ASTER;
  
  (void)sub;
  (void)mul;

  ast_node_base_t *intexpr1 = leaf_wdata(ANS_INTVAL, t1);
  ast_node_base_t *floatexpr1 = leaf_wdata(ANS_FLOATVAL, t3);
  ast_node_base_t *floatexpr2 = leaf_wdata(ANS_FLOATVAL, t4);
  ast_node_base_t *strexpr2 = leaf_wdata(ANS_STR, t6);
  
  ast_node_base_t *intfloatadd =
    node2_wdata(ANS_BINEXPR, add, intexpr1, floatexpr2);
  plvalue_t r1 = eval_expr(intfloatadd, &stack);
  VK_ASSERT_EQUALS(ROC_TEMP, r1.roc);
  VK_ASSERT_EQUALS(JT_FLOAT, r1.pvt);
  VK_ASSERT_EQUALS_F(4396.777, r1.data.fvalue);
  
  ast_node_base_t *intstradd =
    node2_wdata(ANS_BINEXPR, add, intexpr1, strexpr2);
  plvalue_t r2 = eval_expr(intstradd, &stack);
  VK_ASSERT_EQUALS(ROC_TEMP, r2.roc);
  VK_ASSERT_EQUALS(JT_STR, r2.pvt);
  VK_ASSERT_EQUALS(create_string("4396, HJYZ!"), r2.data.svalue);
  
  ast_node_base_t *floatstradd = 
    node2_wdata(ANS_BINEXPR, add, floatexpr1, strexpr2);
  plvalue_t r3 = eval_expr(floatstradd, &stack);
  VK_ASSERT_EQUALS(ROC_TEMP, r3.roc);
  VK_ASSERT_EQUALS(JT_STR, r3.pvt);
  VK_ASSERT_EQUALS(create_string("7777, HJYZ!"), r3.data.svalue);
  
  stack_exit_frame(&stack);
  close_stack(&stack);
  
  VK_TEST_SECTION_END("evaluate with coercion")
}

void test_eval_intermix_storage() {
  VK_TEST_SECTION_BEGIN("evaluate with intermixing storage")
  
  plstack_t stack;
  init_stack(&stack);
  stack_enter_frame(&stack);

  plstkobj_t *stack_a = stack_get(&stack, create_string("a"));
  stack_a->soid = SOID_INT;
  stack_a->value.ivalue = 700;
  
  plheapobj_t *heapobj = heap_alloc_int(70);
  plstkobj_t *stack_b = stack_get(&stack, create_string("b"));
  stack_b->soid = SOID_REF;
  stack_b->value.pvalue = (void*)heapobj;
  
  jjvalue_t t1, t2, t3;
  t1.svalue = create_string("a");
  t2.svalue = create_string("b");
  t3.ivalue = 7;

  jjvalue_t add;
  add.ivalue = TK_ESYM_PLUS;

  ast_node_base_t *expr_a = leaf_wdata(ANS_IDREF, t1);
  ast_node_base_t *expr_b = leaf_wdata(ANS_IDREF, t2);
  ast_node_base_t *expr_7 = leaf_wdata(ANS_INTVAL, t3);
  
  ast_node_base_t *e1 = node2_wdata(ANS_BINEXPR, add, expr_a, expr_b);
  ast_node_base_t *expr = node2_wdata(ANS_BINEXPR, add, e1, expr_7);

  plvalue_t r = eval_expr(expr, &stack);
  VK_ASSERT_EQUALS(ROC_TEMP, r.roc);
  VK_ASSERT_EQUALS(JT_INT, r.pvt);
  VK_ASSERT_EQUALS(777, r.data.ivalue); 

  stack_exit_frame(&stack);
  close_stack(&stack);
  
  VK_TEST_SECTION_END("evaluate with intermixing storage")
}

int main() {
  VK_TEST_BEGIN
  init_heap();
  
  test_eval_idref();
  test_eval_literal();
  test_eval_binary_basic();
  test_eval_coercion();
  test_eval_intermix_storage();
  
  close_heap();
  VK_TEST_END
  return 0;
}
