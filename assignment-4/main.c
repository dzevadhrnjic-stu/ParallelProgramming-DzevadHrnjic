#define _POSIX_C_SOURCE 199309L

#include "sum_algorithms.h"

#include <errno.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef double (*double_sum_function)(const double *, size_t);

typedef struct {
    const char *name;
    double_sum_function function;
} double_method;

static double elapsed_seconds(const struct timespec *start,
                              const struct timespec *end)
{
    time_t seconds = end->tv_sec - start->tv_sec;
    long nanoseconds = end->tv_nsec - start->tv_nsec;
    return (double)seconds + (double)nanoseconds / 1.0e9;
}

static int parse_size(const char *text, size_t *value)
{
    char *end = NULL;
    unsigned long long parsed;

    errno = 0;
    parsed = strtoull(text, &end, 10);
    if (errno != 0 || end == text || *end != '\0' || parsed == 0 ||
        parsed > (unsigned long long)SIZE_MAX) {
        return 0;
    }

    *value = (size_t)parsed;
    return 1;
}

static void fill_test_data(double *values, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        values[i] = i < count / 2 ? 1.0e-3 : 1.0e-16;
    }
}

static double measure(double_sum_function function, const double *values,
                      size_t count, double *result)
{
    struct timespec start;
    struct timespec end;

    if (clock_gettime(CLOCK_MONOTONIC, &start) != 0) {
        return -1.0;
    }
    *result = function(values, count);
    if (clock_gettime(CLOCK_MONOTONIC, &end) != 0) {
        return -1.0;
    }

    return elapsed_seconds(&start, &end);
}

int main(int argc, char **argv)
{
    size_t count = 1U << 20;
    double *values;
    long double reference;
    const double_method methods[] = {
        {"Pairwise", pairwise_sum},
        {"Kahan", kahan_sum},
        {"Knuth", knuth_sum}
    };

    if (argc > 2 || (argc == 2 && !parse_size(argv[1], &count))) {
        fprintf(stderr, "Usage: %s [number_of_values]\n", argv[0]);
        return EXIT_FAILURE;
    }

    values = malloc(count * sizeof(*values));
    if (values == NULL) {
        fprintf(stderr, "Could not allocate %zu values\n", count);
        return EXIT_FAILURE;
    }

    fill_test_data(values, count);
    reference = long_double_sum(values, count);

    printf("Global sum benchmark\n");
    printf("Values: %zu\n", count);
    printf("Reference (long double): %.18Lg\n\n", reference);
    printf("Method,Result,AbsoluteError,RelativeError,Runtime_ms\n");

    {
        struct timespec start;
        struct timespec end;
        long double result;
        double seconds;

        clock_gettime(CLOCK_MONOTONIC, &start);
        result = long_double_sum(values, count);
        clock_gettime(CLOCK_MONOTONIC, &end);
        seconds = elapsed_seconds(&start, &end);
        printf("Long double,%.18Lg,%.18Lg,%.18Lg,%.6f\n",
               result, result - reference, (result - reference) / reference,
               seconds * 1000.0);
    }

    for (size_t i = 0; i < sizeof(methods) / sizeof(methods[0]); ++i) {
        double result;
        double seconds = measure(methods[i].function, values, count, &result);
        long double difference = (long double)result - reference;

        if (seconds < 0.0) {
            fprintf(stderr, "Timer failure while measuring %s\n",
                    methods[i].name);
            free(values);
            return EXIT_FAILURE;
        }
        if (isnan(result)) {
            fprintf(stderr, "Allocation failure while measuring %s\n",
                    methods[i].name);
            free(values);
            return EXIT_FAILURE;
        }

        printf("%s,%.17g,%.18Lg,%.18Lg,%.6f\n",
               methods[i].name, result, difference, difference / reference,
               seconds * 1000.0);
    }

    free(values);
    return EXIT_SUCCESS;
}
