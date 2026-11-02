# Assignment 4 - Parallel Algorithms and Patterns

## Objective

This assignment measures the accuracy and execution time of four global-sum
techniques:

- **Long double:** performs the serial accumulation with a wider floating-point
  type and is used as the reference result.
- **Pairwise summation:** combines values in a balanced tree. Values with
  similar magnitudes are added before partial sums are combined, reducing
  rounding-error growth and making the operation suitable for parallel
  reduction.
- **Kahan summation:** keeps a compensation value for the part of each
  floating-point operation that was rounded away.
- **Knuth summation:** uses error-free transformations to keep the rounding
  error of each addition in a correction value.

The test data deliberately places very large positive values before very small
positive values. This exposes the loss of low-order bits that can occur when a
normal double-precision sum adds values with very different magnitudes.

## Building and running

The argument is the number of values and defaults to `1048576`. The program
prints CSV-compatible output with the method, result, absolute error, relative
error, and runtime in milliseconds.

`Method, Result, AbsoluteError, RelativeError, Runtime_ms`

The three compensated methods return a `double`, while the reference uses
`long double`; therefore their remaining error also includes the final
conversion back to `double`.

## Why the results differ

IEEE-754 floating-point addition is not associative:
`(a + b) + c` can differ from `a + (b + c)`. A serial sum and a parallel
reduction therefore can produce different rounded values even when they use
the same input. The difference becomes more visible when values have different
magnitudes or when cancellation removes significant digits.

Pairwise summation reduces the depth of the accumulation tree. Kahan and Knuth
retain information lost by ordinary additions, so they normally produce a
smaller error than a simple double sum. Long double uses more significand bits,
so it is the most accurate reference on platforms where it provides greater
precision than double. These improvements cost additional operations or memory,
which can increase runtime.

In a parallel program, each worker calculates a local partial sum and a
reduction combines those partial sums. Scheduling, processor count, and the
reduction tree change the order of additions. Since addition is not associative,
the final result is not necessarily reproducible. A fixed reduction tree,
pairwise summation, compensated summation, or a wider accumulator can improve
reproducibility and numerical stability.
