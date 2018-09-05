#ifndef LEXER_H
#define LEXER_H

#include <stdint.h>

typedef enum {
  TK_EOI = -1,
  TK_KWD_FUNCTION,
  TK_KWD_TAKES,
  TK_KWD_RETURNS,
  TK_KWD_BEGIN,
  TK_KWD_END,
  TK_ID,
  TK_NUM_INT,
  TK_NUM_FLOAT,
  TK_SYM_ADD,
  TK_SYM_SUB,
  TK_SYM_MUL,
  TK_SYM_DIV,
  TK_SYM_MOD,
  TK_SYM_AND,
  TK_SYM_OR,
  TK_SYM_NOT,
  TK_SYM_BITAND,
  TK_SYM_BITOR,
  TK_SYM_BITREV
} token_kind;

typedef union {
  int64_t str_handle;
  int64_t ivalue;
  double fvalue;
  void *p_other_value;
} attached_info;

// yylex return value format
// [64bits]
// [<----24bits----> <--16bits--> <--16bits--> <1bit> <-7bits->
// |                 |            |            |      |
// |                 |            |            |      token kind
// |                 |            |            is replacement
// |                 |            column
// |                 row
// attached information address
//
// note: the return value of yylex is not readable directly. It must
// be decomposited by the following functions.
int64_t yylex(void);

int16_t get_value();

#ifdef YY_DECL
#undef YY_DECL
#endif

#define YY_DECL int64_t yylex(void);

#endif
