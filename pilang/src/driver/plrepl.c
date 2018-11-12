#define TEST

#include "ast.h"
#include "config.h"
#include "eval.h"
#include "y.tab.h"
#include "heap.h"
#include "stack.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

int yyparse(void);
int yylex(void);

extern ast_node_base_t *glob_ast;
extern FILE *fp_lex_in;

int main() {
  eprintf("Interactive REPL for %s -- %s\n",
          BAS_LANGUAGE_NAME, BAS_DISCRIPTION);
  eprintf("Created by %s, licensed under %s\n",
          BAS_AUTHOR, BAS_LICENSE);
  eprintf("Distribution \"%s\"\n", VER_NAME);
  eprintf("  Fronted version %d.%d.%d\n",
          VER_PLCFRONT_MAJOR, VER_PLCFRONT_MINOR, VER_PLCFRONT_REVISE);
  eprintf("  Intepreter version %d.%d.%d\n",
          VER_PLI_MAJOR, VER_PLI_MINOR, VER_PLI_REVISE);
  fputc('\n', stderr);

  init_heap();

  stack_t stack;
  init_stack(&stack);
  stack_enter_frame(&stack);

  fp_lex_in = stdin;
  eprintf0("note: for each time you finish input,"
           " type a # character and then return\n");
  eprintf0("empty input with # to exit\n");
  int count = 0;
  while (1) {
    eprintf0("PiLang> ");

    glob_ast = NULL;
    yyparse();
    if (glob_ast == NULL) {
      eprintf0("Parsing error, retry.\n");
      continue;
    }
    else if (glob_ast->node_sema_info == ANS_FUNCTIONS) {
      eprintf0("\nMoriturus te saluto.\n");
      break;
    }

    plvalue_t result = eval_expr(glob_ast, &stack);
    jjvalue_t *storage = fetch_storage(&result);

    eprintf(" %d ] => ", count);
    switch (result.type) {
    case JT_INT:
      eprintf("%" PRId64 " :: Int", storage->ivalue);
      break;
    case JT_FLOAT:
      eprintf("%f :: Float", storage->fvalue);
      break;
    case JT_BOOL:
      eprintf("%s :: Bool", storage->bvalue ? "True" : "False");
      break;
    case JT_STR:
      eprintf("\'%s\' :: Str", get_string(storage->svalue));
      break;
    case JT_LIST:
      eprintf0("(Not printable) :: List");
      break;
    case JT_UNDEFINED:
      eprintf0("Undefined :: Undefined");
      break;

    default:
      eprintf0("error");
    }
    putchar('\n');
    ++count;
  }

  stack_exit_frame(&stack);
  close_stack(&stack);
  close_heap();
}
