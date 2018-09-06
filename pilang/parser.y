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

ast_node_base_t *glob_ast = NULL;

%}

%token TK_EXPR_PLACEHOLDER

%token TK_FUNCTION TK_TAKES TK_RETURNS TK_BEGIN TK_END TK_IF TK_THEN
%token TK_ELSE TK_WHILE TK_FOR
%token TK_ID
%token TK_NUM_INT TK_NUM_FLOAT TK_STR
%token TK_SYM_COMMA TK_SYM_SEMI TK_SYM_DOT
%token TK_SYM_LBRACKET TK_SYM_RBRACKET
%token TK_ESYM_EQ TK_ESYM_EQEQ TK_ESYM_PLUS TK_ESYM_MINUS TK_ESYM_ASTER
%token TK_ESYM_AMP TK_ESYM_PIPE TK_ESYM_AMPAMP TK_ESYM_PIPEPIPE
%token TK_ESYM_CARET TK_ESYM_SLASH TK_ESYM_PERCENT TK_ESYM_LPAREN 
%token TK_ESYM_RPAREN TK_ESYM_LBRACE TK_ESYM_RBRACE TK_ESYM_LT TK_ESYM_GT
%token TK_ESYM_NOT TK_ESYM_NEQ TK_ESYM_LEQ TK_ESYM_GEQ

%%

program: functions { glob_ast = $1.ast; $$ = $1; }

functions: 
  functions function { $$.ast = node2(ANS_LIST, $1.ast, $2.ast); } | ;

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
    $$.ast = node2(ANS_LIST, $1.ast, $2.ast);
  }
  |
  ;

statement: empty_statement { $$ = $1; } |
           expr_statement { $$ = $1; } 
           if_statement { $$ = $1; }
           while_statement { $$ = $1; }
//           for_statement { $$ = $1; } ;

expr_statement: expr { $$ = $1; } ;

if_statement: 
  TK_IF expr TK_THEN statements TK_END TK_IF
  {
    $$.ast = node2(ANS_IF, $2.ast, $4.ast);
  }
  |
  TK_IF expr TK_THEN statements TK_ELSE statements TK_END TK_IF
  {
    $$.ast = node3(ANS_IF, $2.ast, $4.ast, $6.ast);
  }
  ;

while_statement: TK_WHILE expr TK_THEN statements TK_END TK_WHILE
  {
    $$.ast = node2(ANS_WHILE, $2.ast, $4.ast);
  }
  ;

expr: TK_EXPR_PLACEHOLDER;

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
  fprintf(stderr, "at line %d, col %d: error: %s\n", yylval.token.row,
          yylval.token.col, err);
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
  if (fp_lex_in != stdin) {
    fclose(fp_lex_in);
  }

  printf("glob_ast = %p\n", glob_ast);
}
