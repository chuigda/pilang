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
} plstkframe_t;

typedef struct {
  plstkobj_t *storage;
  int32_t stack_size;
  int32_t stack_usage;
  list_t frames;
} plstack_t;

void init_stack(plstack_t *stack);

void stack_enter_frame(plstack_t *stack);

void stack_exit_frame(plstack_t *stack);

plstkobj_t *stack_get(plstack_t *stack, int64_t name);

void close_stack(plstack_t *stack);

#define DFL_STACK_SIZE 65536
#define DFL_STACKFRAME_COUNT 512

#endif
