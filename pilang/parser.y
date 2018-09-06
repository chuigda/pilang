%{

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "mstring.h"
#include "jjvalue.h"
#include "ast.h"

#include "yystype.h"

extern int yylex(void);
extern void yyerror(const char *err);

%}

%token TK_FUNCTION TK_TAKES TK_RETURNS TK_BEGIN TK_END
%token TK_ID
%token TK_NUM_INT TK_NUM_FLOAT TK_STR
%token TK_SYM_COMMA TK_SYM_SEMI TK_SYM_DOT
%token TK_ESYM_LBRACKET TK_ESYM_RBRACKET TK_ESYM_EQ 
%token TK_ESYM_PLUS TK_ESYM_MINUS TK_ESYM_ASTER TK_ESYM_SLASH
%token TK_ESYM_AMP TK_ESYM_PIPE TK_ESYM_CARET

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
    $$.ast = node1(ANS_FUNCTION_BODY, $2.ast);
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

empty_statement: TK_SYM_SEMI { $$.ast = leaf(ANS_NULL); };

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

int main(int argc, char *argv[]) {
  extern FILE *fp_lex_in;
  if (argc == 2) {
    fp_lex_in = fopen(argv[1], "r");
    if (fp_lex_in == NULL) {
      fprintf(stderr, "Cannot open file %s\n", argv[1]);
      exit(-1);
    }
  }
  else if (argc == 1) {
    fp_lex_in = stdin;
  }
  else {
    fprintf(stderr, "Too many arguments\n");
  }
  yyparse();
}
