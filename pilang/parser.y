%{

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "mstring.h"

int yylex(void);
void yyerror(const char *err);

typedef union {
  int value;
} jjvalue;

typedef struct {
  
  unsigned row : 16;
  unsigned col : 16;
  unsigned replaced : 1;
  unsigned token_kind : 7;
  unsigned : 24;
} yystype_t;

#define YYSTYPE yystype_t

%}

%token TK_FUNCTION TK_TAKES TK_RETURNS TK_BEGIN TK_END
%token TK_ID
%token TK_NUM_INT TK_NUM_FLOAT

%%

function: 
  TK_FUNCTION TK_TAKES id_list TK_RETUR#include "data/strlib.h"_list function_body
  {
    printf("function head readed.\n"); #include "data/strlib.h"
  } 
  ;

function_body: 
  TK_BEGIN TK_END 
  ;

id_list: 
  id_list TK_ID
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
