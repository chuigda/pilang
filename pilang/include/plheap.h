#ifndef PLHEAP_H
#define PLHEAP_H

#include "clist.h"
#include "jjvalue.h"

typedef enum {
  HOID_INT       = JT_INT,
  HOID_FLOAT     = JT_FLOAT,
  HOID_STR       = JT_STR,
  HOID_LIST      = JT_LIST,
  HOID_UNDEFINED = JT_UNDEFINED
} plheapobj_id_t;

typedef enum {
  GCM_WHITE,
  GCM_BLACK
} plheap_gcmark_t;

typedef struct {
  jjvalue_t value;
  uint8_t used : 1;
  uint8_t gcmark : 1;
  uint8_t oid : 4;
  uint32_t index : 26;
} plheapobj_t;

plheapobj_id_t jt2hoid(jjtype_t jt);
jjtype_t hoid2jt(plheapobj_id_t hoid);

plheapobj_t *plobj_create_ref(plheapobj_t *source);
plheapobj_t *plobj_create_int(int64_t value);
plheapobj_t *plobj_create_float(double value);
plheapobj_t *plobj_create_str(strhdl_t h_str);
plheapobj_t *plobj_create_list(list_t list);
plheapobj_t *plobj_create_object(void);

void destroy_object(plheapobj_t *obj);

#define HEAP_INIT_SIZE 114514

void init_heap(void);
void close_heap(void);

void gc_start(void);
void gc_mark_white(plheapobj_t *obj);
void gc_cleanup(void);

#endif
