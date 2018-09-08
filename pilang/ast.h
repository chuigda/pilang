#ifndef AST_H
#define AST_H

#include <stdint.h>
#include "jjvalue.h"

typedef enum {
  #define ANK_DECLARE(KEY) KEY,
  #include "ast_node_kind.h"
  ANK_INVALID
} ast_node_kind_t;

typedef enum {
  #define ANS_DECLARE(KEY) KEY,
  #include "ast_sema_info.h"
  ANS_INVALID
} ast_node_sema_t;

#define AST_NODE_COMMON \
  uint16_t node_kind; \
  uint16_t node_sema_info; \
  uint16_t node_uid; \
  uint16_t : 16; // padding

typedef struct {
  AST_NODE_COMMON
} ast_node_base_t;

typedef struct {
  AST_NODE_COMMON
  jjvalue_t data;
} ast_node_wdata_base_t;

_Static_assert(sizeof(ast_node_base_t) == 8,
               "Unexpected size difference");

_Static_assert(sizeof(ast_node_wdata_base_t) == 16,
               "Unexpected size difference");

typedef struct {
  AST_NODE_COMMON
} ast_leaf_t;

typedef struct {
  AST_NODE_COMMON
  jjvalue_t data;
} ast_leaf_wdata_t;

typedef struct {
  AST_NODE_COMMON
  ast_node_base_t *child;
} ast_schild_t;

typedef struct {
  AST_NODE_COMMON
  jjvalue_t data;
  ast_node_base_t *child;
} ast_schild_wdata_t;

typedef struct {
  AST_NODE_COMMON
  ast_node_base_t *children[2];
} ast_dchild_t;

typedef struct {
  AST_NODE_COMMON
  jjvalue_t data;
  ast_node_base_t *children[2];
} ast_dchild_wdata_t;

typedef struct {
  AST_NODE_COMMON
  ast_node_base_t *children[3];
} ast_tchild_t;

typedef struct {
  AST_NODE_COMMON
  jjvalue_t data;
  ast_node_base_t *children[3];
} ast_tchild_wdata_t;

extern ast_node_base_t *leaf(ast_node_sema_t sema_info);

extern ast_node_base_t *leaf_wdata(ast_node_sema_t sema_info, 
                                   jjvalue_t data);

extern ast_node_base_t *node1(ast_node_sema_t sema_info, 
                              ast_node_base_t *child);

extern ast_node_base_t *node1_wdata(ast_node_sema_t sema_info, 
                                    jjvalue_t data,
                                    ast_node_base_t *child);

extern ast_node_base_t *node2(ast_node_sema_t sema_info, 
                              ast_node_base_t *lchild, 
                              ast_node_base_t *rchild);

extern ast_node_base_t *node2_wdata(ast_node_sema_t sema_info,
                                    jjvalue_t data,
                                    ast_node_base_t *lchild,
                                    ast_node_base_t *rchild);

extern ast_node_base_t *node3(ast_node_sema_t sema_info,
                              ast_node_base_t *child0,
                              ast_node_base_t *child1,
                              ast_node_base_t *child2);

extern ast_node_base_t *node3_wdata(ast_node_sema_t sema_info,
                                    jjvalue_t data,
                                    ast_node_base_t *child0,
                                    ast_node_base_t *child1,
                                    ast_node_base_t *child2);

extern void tree_print(ast_node_base_t *root, uint16_t parent);

#endif

