#ifndef YYSTYPE_H
#define YYSTYPE_H

#include "jjvalue.h"
#include "ast.h"

typedef struct {
  jjvalue_t val;
  unsigned row : 16;
  unsigned col : 16;
  unsigned replaced : 1;
  unsigned token_kind : 7;
  unsigned : 24;
} token_t;

typedef union {
  token_t token;
  ast_node_base_t *ast;
} yystype_t;

#define YYSTYPE yystype_t

#endif