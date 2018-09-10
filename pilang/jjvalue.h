#ifndef JJVALUE_H
#define JJVALUE_H

#include "clist.h"

#include <stdint.h>

typedef union {
  int64_t ivalue;
  double fvalue;
  int64_t svalue;
  list_t lsvalue;
  void *pvalue;
} jjvalue_t;

#endif
