#include "stack.h"

#include "util.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static plstkobj_t *stack_allocate(plstack_t *stack, int64_t name) {
  if (stack->stack_usage == stack->stack_size) {
    fprintf(stderr, "pilang pivm: stack overflow, "
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

static void stack_allocate_n(plstack_t *stack, size_t n,
                             plstkobj_t **begin, plstkobj_t **end) {
  plstkobj_t *first = stack_allocate(stack, -1);
  for (int i = 1; i < n; i++) {
    stack_allocate(stack, -1);
  }
  *begin = first;
  *end = first + n;
}

static plstkobj_t *frame_get(plstkframe_t *frame, int64_t name) {
  for (plstkobj_t *obj = frame->objs_begin;
       obj != frame->objs_end;
       ++obj) {
    if (name == obj->name) {
      return obj;
    }
  }
  return NULL;
}

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

void stack_exit_frame(plstack_t *stack) {
  plstkframe_t *curframe =
    (plstkframe_t*)iter_deref(iter_prev(list_end(&(stack->frames))));
  stack->stack_usage -= (curframe->objs_end - curframe->objs_begin);
  list_remove(&(stack->frames), iter_prev(list_end(&(stack->frames))));
}

plstkobj_t *stack_get(plstack_t *stack, int64_t name) {
  for (iter_t it = iter_prev(list_end(&(stack->frames)));
       !iter_eq(iter_prev(list_begin(&(stack->frames))), it);
       it = iter_prev(it)) {
    plstkobj_t *find_result = frame_get((plstkframe_t*)iter_deref(it),
                                        name);
    if (find_result != NULL) {
      return find_result;
    }
  }

  plstkobj_t *result = stack_allocate(stack, name);
  result->soid = SOID_UNDEFINED;
  return result;
}
