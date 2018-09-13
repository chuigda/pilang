#ifndef STACK_H
#define STACK_H

#include "plheap.h"

#include "clist.h"
#include "mstring.h"

#include <stddef.h>
#include <stdint.h>

typedef enum {
  SOID_INT,
  SOID_FLOAT,
  SOID_REF,
  SOID_UNDEFINED
} pstkobj_id_t;

typedef struct {
  int64_t name;
  union {
    int64_t ivalue;
    double fvalue;
    plobj_t *refto;
  } value;
  int8_t soid;
} plstkobj_t;

typedef struct {
  plstkobj_t *objs_begin, *objs_end;
  plstkobj_t *params_begin, *params_end;
  plstkobj_t *returns_begin, *returns_end;
} plstkframe_t;

typedef struct {
  plstkobj_t *storage;
  int32_t stack_size;
  int32_t stack_usage;
  list_t frames;
} plstack_t;

void init_stack(plstack_t *stack);

void stack_enter_frame(plstack_t *stack,
                       size_t param_count,
                       size_t returns_count);

// FIXME: how to initialize parameters and return values (at least their
//        names properly?
// void stack_init_param(size_t nth, int64_t name, plstkobj_t *object);
// void stack_init_return(size_t nth, int64_t name);

void stack_exit_frame(plstack_t *stack);

plstkobj_t *stack_get(plstack_t *stack, int64_t name);

#define DFL_STACK_SIZE 65536
#define DFL_STACKFRAME_COUNT 512

#endif
