#ifndef JJVALUE_H
#define JJVALUE_H

#include "clist.h"
#include "mstring.h"

#include <stdint.h>

typedef union {
  int64_t ivalue;
  double fvalue;
  int64_t svalue;
  list_t lsvalue;
  void *pvalue;
} jjvalue_t;

typedef struct {
  jjvalue_t value;
  bool success;
} result_t;

static inline result_t failed_result(const char *error) {
  result_t r;
  r.success = 0;
  r.value.svalue = create_string(error);
  return r;
}

static inline result_t success_result(jjvalue_t value) {
  result_t r;
  r.success = 1;
  r.value = value;
  return r;
}

#endif
