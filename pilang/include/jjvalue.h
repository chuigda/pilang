#ifndef JJVALUE_H
#define JJVALUE_H

#include "clist.h"
#include "mstring.h"

#include <stdint.h>

#define TP(...)
#define TEMPLATE(...)

typedef union {
  int64_t ivalue;
  double fvalue;
  int64_t svalue;
  list_t lsvalue;
  void *pvalue;
} jjvalue_t;

typedef enum {
  JT_INT,
  JT_FLOAT,
  JT_STR,
  JT_LIST,
  JT_REF,
  JT_UNDEFINED
} jjtype_t;

TEMPLATE(T)
typedef struct {
  jjvalue_t TP(T) value;
  bool success;
} result_t;

TEMPLATE(T)
static inline result_t TP(T) failed_result(const char *error) {
  result_t TP(T) r;
  r.success = 0;
  r.value.svalue = create_string(error);
  return r;
}

TEMPLATE(T)
static inline result_t TP(T) success_result(jjvalue_t TP(T) value) {
  result_t TP(T) r;
  r.success = 1;
  r.value = value;
  return r;
}

#endif
