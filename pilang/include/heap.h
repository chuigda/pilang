#ifndef HEAP_H
#define HEAP_H

#include "clist.h"
#include "jjvalue.h"

typedef enum {
  HOID_INT       = JT_INT,
  HOID_FLOAT     = JT_FLOAT,
  HOID_STR       = JT_STR,
  HOID_LIST      = JT_LIST,
  HOID_UNDEFINED = JT_UNDEFINED
} heapobj_id_t;

typedef enum {
  GCM_WHITE,
  GCM_BLACK
} heap_gcmark_t;

typedef struct {
  jjvalue_t value;
  uint8_t used : 1;
  uint8_t gcmark : 1;
  uint8_t oid : 4;
  uint32_t index : 26;
} heapobj_t;

heapobj_id_t jt2hoid(jjtype_t jt);
jjtype_t hoid2jt(heapobj_id_t hoid);

heapobj_t *heap_alloc_ref(heapobj_t *source);
heapobj_t *heap_alloc_int(int64_t value);
heapobj_t *heap_alloc_float(double value);
heapobj_t *heap_alloc_str(strhdl_t h_str);
heapobj_t *heap_alloc_list(list_t list);

void destroy_object(heapobj_t *obj);

void init_heap(void);
void close_heap(void);

void gc_start(void);
void gc_mark_white(heapobj_t *obj);
void gc_cleanup(void);

#endif
