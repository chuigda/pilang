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
  jjvalue val;
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

%%

function: 
  TK_FUNCTION TK_TAKES id_list TK_RETURNS id_list function_body
  {
    printf("function head readed.\n");
  } 
  ;

function_body: 
  TK_BEGIN TK_END 
  ;

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
