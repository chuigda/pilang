#ifndef AST_H
#define AST_H

#include <stdint.h>

#include "jjvalue.h"

typedef enum {
  ANK_LEAF,
  ANK_SINGLE_CHILD,
  ANK_DUAL_CHILD,
  ANK_TRIPLE_CHILD,
  ANK_LEAF_WDATA,
  ANK_SINGLE_CHILD_WDATA,
  ANK_DUAL_CHILD_WDATA,
  ANK_TRIPLE_CHILD_WDATA
} ast_node_kind_t;

typedef enum {
  ANS_LIST,
  ANS_ID
} ast_node_sema_t;

#define AST_NODE_COMMON \
  uint32_t node_kind : 16; \
  uint32_t node_sema : 16; \
  uint32_t : 32;

typedef struct {
  AST_NODE_COMMON
} ast_node_base_t;

extern int leaf_node(ast_node_sema_t sema);
extern int leaf_node_wdata(ast_node_sema_t sema, jjvalue data);
extern int node1(ast_node_sema_t sema, int child);
extern int node2(ast_node_sema_t sema, int lchild, int rchild);

#endif