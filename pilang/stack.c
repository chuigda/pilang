#include "stack.h"

#include "util.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_stack(plstack_t *stack) {
  stack->storage = NEWN(plstkobj_t, DFL_STACK_SIZE);
  stack->stack_size = DFL_STACK_SIZE;
  stack->stack_usage = 0;
  create_list(&(stack->frames), malloc, free);
}

void stack_enter_frame(plstack_t *stack, size_t param_count,
                       size_t return_count) {
  plstkframe_t *frame = NEW(plstkframe_t);
  list_push_back(&(stack->frames), frame);
  stack_allocate_n(stack, param_count, &(frame->params_begin),
                   &(frame->params_end));
  stack_allocate_n(stack, return_count, &(frame->returns_begin),
                   &(frame->returns_end));
}

plstkobj_t *stack_allocate(plstack_t *stack, const char *name) {
  char *copied_name;
  if (name == NULL) {
    copied_name = NULL;
  }
  else {
    copied_name = NEWN(char, strlen(name) + 1);
    strcpy(copied_name, name);
  }

  if (stack->stack_usage == stack->stack_size) {
    fprintf(stderr, "pilang pivm: stack overflow, "
                    "with DFL_STACK_SIZE = %d\n", DFL_STACK_SIZE);
    abort();
  }

  plstkobj_t *obj = stack->storage + stack->stack_usage;
  obj->name = copied_name;

  plstkframe_t *cur_frame = 
    (plstkframe_t*)iter_deref(iter_prev(list_end(&(stack->frames))));
  stack->stack_usage++;
  cur_frame->objs_end++;
  return obj;
}

void stack_allocate_n(plstack_t *stack, size_t n,
                      plstkobj_t **begin, plstkobj_t **end) {
  plstkobj_t *first = stack_allocate(stack, NULL);
  for (int i = 1; i < n; i++) {
    stack_allocate(stack, NULL);
  }
  *begin = first;
  *end = first + n;
}

