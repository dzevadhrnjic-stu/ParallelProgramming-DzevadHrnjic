#include "sum_algorithms.h"

#include <math.h>
#include <stdlib.h>

long double long_double_sum(const double *values, size_t count)
{
    long double sum = 0.0L;

    for (size_t i = 0; i < count; ++i) {
        sum += (long double)values[i];
    }

    return sum;
}

double pairwise_sum(const double *values, size_t count)
{
    if (count == 0) {
        return 0.0;
    }

    double *partial = malloc(count * sizeof(*partial));
    if (partial == NULL) {
        return NAN;
    }

    for (size_t i = 0; i < count; ++i) {
        partial[i] = values[i];
    }

    size_t active = count;
    while (active > 1) {
        size_t next_active = (active + 1) / 2;
        for (size_t i = 0; i < active / 2; ++i) {
            partial[i] = partial[2 * i] + partial[2 * i + 1];
        }
        if (active % 2 != 0) {
            partial[next_active - 1] = partial[active - 1];
        }
        active = next_active;
    }

    double result = partial[0];
    free(partial);
    return result;
}

double kahan_sum(const double *values, size_t count)
{
    double sum = 0.0;
    double correction = 0.0;

    for (size_t i = 0; i < count; ++i) {
        double adjusted = values[i] - correction;
        double next_sum = sum + adjusted;
        correction = (next_sum - sum) - adjusted;
        sum = next_sum;
    }

    return sum;
}

double knuth_sum(const double *values, size_t count)
{
    double sum = 0.0;
    double correction = 0.0;

    for (size_t i = 0; i < count; ++i) {
        double first = sum;
        double second = values[i] + correction;
        double combined = first + second;
        double rounded_first = combined - second;
        double rounded_second = combined - rounded_first;

        sum = combined;
        correction = (first - rounded_first) + (second - rounded_second);
    }

    return sum + correction;
}
