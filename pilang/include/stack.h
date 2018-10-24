#ifndef STACK_H
#define STACK_H

#include "clist.h"
#include "mstring.h"
#include "plheap.h"

#include <stddef.h>
#include <stdint.h>

typedef enum {
  SOID_INT       = JT_INT,
  SOID_FLOAT     = JT_FLOAT,
  SOID_STR       = JT_STR,
  SOID_REF       = JT_REF,
  SOID_UNDEFINED = JT_UNDEFINED
} plstkobj_id_t;

typedef struct {
  jjvalue_t value;
  strhdl_t name;
  int8_t soid;
} plstkobj_t;

typedef struct {
  plstkobj_t *objs_begin, *objs_end;
} plstkframe_t;

typedef struct {
  plstkobj_t *storage;
  int32_t stack_size;
  int32_t stack_usage;
  list_t TP(plstkframe_t) frames;
} plstack_t;

plstkobj_id_t jt2soid(jjtype_t jt);
jjtype_t soid2jt(plstkobj_id_t soid);

void init_stack(plstack_t *stack);

void close_stack(plstack_t *stack);

void stack_enter_frame(plstack_t *stack);

void stack_exit_frame(plstack_t *stack);

plstkobj_t *stack_get(plstack_t *stack, strhdl_t name);

void request_stack_gc(plstack_t *stack);

#define DFL_STACK_SIZE 65536
#define DFL_STACKFRAME_COUNT 512

#endif
