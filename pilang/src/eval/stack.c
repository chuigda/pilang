#include "stack.h"

#include "util.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static plstkobj_t *stack_allocate(plstack_t *stack, int64_t name) {
  if (stack->stack_usage == stack->stack_size) {
    eprintf("pilang pivm: stack overflow, "
            "with DFL_STACK_SIZE = %d\n", DFL_STACK_SIZE);
    abort();
  }

  plstkobj_t *obj = stack->storage + stack->stack_usage;
  obj->name = name;

  plstkframe_t *cur_frame = 
    (plstkframe_t*)iter_deref(iter_prev(list_end(&(stack->frames))));
  stack->stack_usage++;
  cur_frame->objs_end++;
  return obj;
}

void init_stack(plstack_t *stack) {
  stack->storage = NEWN(plstkobj_t, DFL_STACK_SIZE);
  stack->stack_size = DFL_STACK_SIZE;
  stack->stack_usage = 0;
  create_list(&(stack->frames), malloc, free);
}

void stack_enter_frame(plstack_t *stack) {
  plstkframe_t *frame = NEW(plstkframe_t);
  frame->objs_begin = stack->storage + stack->stack_usage;
  frame->objs_end = stack->storage + stack->stack_usage;
  list_push_back(&(stack->frames), frame);
}

void stack_exit_frame(plstack_t *stack) {
  plstkframe_t *curframe =
    (plstkframe_t*)iter_deref(iter_prev(list_end(&(stack->frames))));
  stack->stack_usage -= (curframe->objs_end - curframe->objs_begin);
  list_remove(&(stack->frames), iter_prev(list_end(&(stack->frames))));
  free(curframe);
}

plstkobj_t *stack_get(plstack_t *stack, int64_t name) {
  plstkframe_t *frame = 
    (plstkframe_t*)iter_deref(iter_prev(list_end(&(stack->frames))));

  for (plstkobj_t *obj = frame->objs_begin;
       obj != frame->objs_end;
       ++obj) {
    if (name == obj->name) {
      return obj;
    }
  }
  
  plstkobj_t *obj = stack_allocate(stack, name);
  obj->soid = SOID_UNDEFINED;
  return obj;
}

void close_stack(plstack_t *stack) {
  destroy_list(&(stack->frames));
  free(stack->storage);
}
