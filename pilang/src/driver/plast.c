#include "ast.h"
#include "tree_dump.h"
#include "y.tab.h"

#include <stdio.h>
#include <stdlib.h>

int yyparse(void);
int yylex(void);

extern ast_node_base_t *glob_ast;

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
  tree_dump(glob_ast);
}
