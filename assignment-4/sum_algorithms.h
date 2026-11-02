#ifndef SUM_ALGORITHMS_H
#define SUM_ALGORITHMS_H

#include <stddef.h>

long double long_double_sum(const double *values, size_t count);
double pairwise_sum(const double *values, size_t count);
double kahan_sum(const double *values, size_t count);
double knuth_sum(const double *values, size_t count);

#endif
