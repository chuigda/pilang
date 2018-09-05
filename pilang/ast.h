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

typedef struct {
  AST_NODE_COMMON
} ast_leaf_t;

typedef struct {
  AST_NODE_COMMON
  jjvalue data;
} ast_leaf_wdata_t;

typedef struct {
  AST_NODE_COMMON
  jjvalue data;
} ast_schild_t;

typedef struct {
  AST_NODE_COMMON
  jjvalue data;
  uint32_t child;
} ast_schild_wdata_t;

typedef struct {
  AST_NODE_COMMON
  uint32_t children[2];
} ast_dchild_t;

typedef struct {
  AST_NODE_COMMON
  jjvalue data;
  uint32_t children[2];
} ast_dchild_wdata_t;

typedef struct {
  AST_NODE_COMMON
  uint32_t children[3];
} ast_tchild_t;

typedef struct {
  AST_NODE_COMMON
  jjvalue data;
  uint32_t children[3];
} ast_tchild_wdata_t;

extern uint32_t leaf(ast_node_sema_t sema);
extern uint32_t leaf_wdata(ast_node_sema_t sema, jjvalue data);
extern uint32_t node1(ast_node_sema_t sema, uint32_t child);
extern uint32_t node1_wdata(ast_node_sema_t sema, jjvalue data,
                            uint32_t child);
extern uint32_t node2(ast_node_sema_t sema, uint32_t lchild, 
                      uint32_t rchild);
extern uint32_t node2_wdata(ast_node_sema_t sema, jjvalue data,
                            uint32_t lchild, uint32_t rchild);
extern uint32_t node3(ast_node_sema_t sema, uint32_t child0, 
                      uint32_t child1, uint32_t child2);
extern uint32_t node3_wdata(ast_node_sema_t sema, jjvalue data,
                            uint32_t child0, uint32_t child1, 
                            uint32_t child2);


#endif
