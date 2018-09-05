#ifndef JJVALUE_H
#define JJVALUE_H

#include <stdint.h>

typedef union {
  int64_t ivalue;
  double fvalue;
  int64_t svalue;
  void *pvalue;
} jjvalue_t;

#endif
