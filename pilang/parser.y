%{

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "mstring.h"
#include "jjvalue.h"
#include "ast.h"

int yylex(void);
void yyerror(const char *err);

typedef struct {
  jjvalue_t val;
  unsigned row : 16;
  unsigned col : 16;
  unsigned replaced : 1;
  unsigned token_kind : 7;
  unsigned : 24;
} token_t;

typedef struct {

} ast_t;

typedef union {
  token_t token;
  uint32_t ast;
} yystype_t;

#define YYSTYPE yystype_t

%}

%token TK_FUNCTION TK_TAKES TK_RETURNS TK_BEGIN TK_END
%token TK_ID
%token TK_NUM_INT TK_NUM_FLOAT
%token TK_SYM_COMMA TK_SYM_SEMI TK_SYM_DOT

%%

function: 
  TK_FUNCTION TK_ID TK_TAKES id_list TK_RETURNS id_list function_body
  {
    $$.ast = node3_wdata(ANS_FUNCTION, $2.token.val, $4.ast, $6.ast,
                         $7.ast);
  } 
  ;

function_body: 
  TK_BEGIN statements TK_END 
  {
    $$.ast = leaf(ANS_NULL);
  }
  ;

statements:
  statements statement
  {
    $$.ast = node2(ANS_STATEMENTS, $1.ast, $2.ast);
  }
  |
  ;

statement: empty_statement { $$ = $1; };

empty_statement: TK_SYM_SEMI { $$ = leaf(ANS_NULL); };

id_list: 
  id_list TK_ID
  {
    $$.ast = node2(ANS_LIST, $1.ast, $2.ast);
  }
  | 
  ;

%%

void yyerror(const char *err) {
  fprintf(stderr, "at line %d, col %d: error: %s\n", err, 0, 0);
}

int yylex() {
  return -1;
}

int main() {
  yyparse();
}
