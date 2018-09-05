#include "ast.h"

uint32_t leaf(ast_node_sema_t sema) {
  return 0;
}

uint32_t node1(ast_node_sema_t sema, uint32_t child) {
  return 0;
}

uint32_t node2(ast_node_sema_t sema, uint32_t lchild, uint32_t rchild) {
  return 0;
}

uint32_t node3(ast_node_sema_t sema, uint32_t child0, uint32_t child1,
               uint32_t child2) {
  return 0;
}

uint32_t node3_wdata(ast_node_sema_t sema, jjvalue_t data, 
                     uint32_t child0, uint32_t child1, uint32_t child2) {
  return 0;
}
