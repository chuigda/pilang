#ifndef PLHEAP_H
#define PLHEAP_H

#include "clist.h"
#include "jjvalue.h"

typedef enum {
  OID_REF,
  OID_INT,
  OID_FLOAT,
  OID_STR,
  OID_LIST,
  OID_SET,
  OID_STRUCT
} plobj_id_t;

typedef enum {
  GCM_WHITE,
  GCM_BLACK
} plobj_gcmark_t;

typedef struct {
  jjvalue_t value;
  uint8_t used : 1;
  uint8_t gcmark : 1;
  uint8_t oid : 4;
  uint32_t index : 26;
} plobj_t;

plobj_t *plobj_create_ref(plobj_t *source);
plobj_t *plobj_create_int(int64_t value);
plobj_t *plobj_create_float(double value);
plobj_t *plobj_create_str(int64_t h_str);
plobj_t *plobj_create_list(list_t list);
plobj_t *plobj_create_object(void);

void destroy_object(plobj_t *obj);

#define HEAP_INIT_SIZE 114514

void init_heap(void);
void close_heap(void);

void gc_start(void);
void gc_mark_white(plobj_t *obj);
void gc_cleanup(void);

#endif
