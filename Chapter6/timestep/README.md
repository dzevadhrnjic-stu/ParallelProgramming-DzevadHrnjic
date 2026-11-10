# Timestep vectorization assignment

This assignment starts from the `Example_03` implementation and compares the
serial loop with the three vectorization improvements from Chapter 6.

| Program | Change |
|---|---|
| `timestep` | Baseline loop with `restrict` on `celltype` |
| `timestep_opt1` | OpenMP SIMD `reduction(min:mymindt)` |
| `timestep_opt2` | SIMD reduction plus explicit `private` temporaries |
| `timestep_opt3` | SIMD reduction plus loop-local temporaries |
| `timestep_opt3_fast` | `timestep_opt3` with `-fno-trapping-math -fno-math-errno` |

All versions use the same input data and must print the same minimum time step.

## Build

For my measurements I used GCC and an AMD Ryzen 5 5500. The CPU
supports AVX2, so the preferred vector width is 256 bits (four `double`
values per SIMD vector):

```text
cmake -S . -B build-linux -DVECTOR_WIDTH=256
cmake --build build-linux --parallel
```

The compiler is invoked with `-O3`, `-fopenmp-simd`, `-ftree-vectorize`,
`-march=native`, `-mprefer-vector-width=256`,
`-fopt-info-vec-optimized`, and `-fopt-info-vec-missed`. Vectorization
diagnostics are printed during the build. To request another width, configure
with `-DVECTOR_WIDTH=128` or `-DVECTOR_WIDTH=512`; the selected width must
match the target CPU when interpreting results.

The equivalent Makefile build is:

```text
make -f Makefile.simple VECTOR_WIDTH=256
```

## Correctness

Run the executables after building:

```text
./build-linux/timestep
./build-linux/timestep_opt1
./build-linux/timestep_opt2
./build-linux/timestep_opt3
./build-linux/timestep_opt3_fast
```

Each executable should print:

```text
Minimum dt is 0.016964
```

The result is unchanged because the reduction computes the minimum rather than
relying on a reassociated sum. The SIMD directive tells the compiler that the
minimum reduction is safe across independent vector lanes.

## Compiler results

`timestep` is the reference implementation and has no SIMD directive. GCC may
still report vectorization analysis for parts of the loop, but the conditional
minimum and math calls can prevent full automatic vectorization.

`timestep_opt1` adds the required `min` reduction. `timestep_opt2` additionally
declares the temporary values private to each SIMD iteration. `timestep_opt3`
expresses the same ownership by declaring those values inside the loop, which
is usually the clearest form.

With a 256-bit preference, a fully vectorized `double` loop uses four values per
vector. On AVX2 this corresponds to 256-bit YMM instructions. A compiler can
choose a smaller width for a particular loop, so the vectorization report and
disassembly are the authoritative evidence.

The extra `timestep_opt3_fast` flags permit more aggressive floating-point
optimization by disabling trapping-math and `errno` requirements for math
functions. They may improve vectorization, but should only be used when the
application accepts those floating-point assumptions.

## My compiler and runtime results

The Linux/GCC build produced the following messages for the timestep kernels:

```text
timestep.c:9:21: missed: couldn't vectorize loop
timestep_opt1.c:9:9: optimized: loop vectorized using 32 byte vectors
timestep_opt2.c:9:9: optimized: loop vectorized using 32 byte vectors
timestep_opt3.c:8:9: optimized: loop vectorized using 32 byte vectors
timestep_opt3.c:10:7: optimized: loop vectorized using 32 byte vectors
```

The `missed` messages for the optimized files refer to the conditional body;
the OpenMP SIMD loop itself is reported as vectorized. The baseline loop is not
fully vectorized. The initialization loop in `main.c` was also vectorized with
32-byte vectors, but it is not the kernel being compared.

All five WSL/Linux executables returned `Minimum dt is 0.016964`. In one
`/usr/bin/time` run on the Windows-mounted WSL filesystem, I measured:

| Program | Elapsed time |
|---|---:|
| `timestep` | 0.54 s |
| `timestep_opt1` | 0.54 s |
| `timestep_opt2` | 0.52 s |
| `timestep_opt3` | 0.50 s |
| `timestep_opt3_fast` | 0.50 s |

These times are from one run, so they can change slightly from run to run. In
my test, `timestep_opt3` and `timestep_opt3_fast` were the fastest versions.
For a more accurate comparison, I would run the programs several times on a
native Linux filesystem.

## LIKWID measurements

`likwid-perfctr` 5.2.2 is installed in the WSL environment and recognizes the
AMD Ryzen 5 5500, but WSL does not expose the MSR registers required for the
hardware counters. Each attempted run reported:

```text
CPU name: AMD Ryzen 5 5500
CPU type: AMD K19 (Zen3) architecture
Cannot get access to MSRs. Please check permissions to the MSRs
```

Because of this, I could not collect the actual LIKWID counter values in WSL.
On a native Linux system, or on a system where MSR access is allowed, I would
use the commands below to measure every version:

```text
likwid-perfctr -C 0 -g FLOPS_DP ./build-linux/timestep
likwid-perfctr -C 0 -g FLOPS_DP ./build-linux/timestep_opt1
likwid-perfctr -C 0 -g FLOPS_DP ./build-linux/timestep_opt2
likwid-perfctr -C 0 -g FLOPS_DP ./build-linux/timestep_opt3
likwid-perfctr -C 0 -g FLOPS_DP ./build-linux/timestep_opt3_fast
```

I would compare the results by looking at the runtime, instructions per cycle,
and double-precision FLOP values. The best version is the one that has the
shortest runtime and gives the same result as the other versions.

For WSL-only timing, use `/usr/bin/time` and label the values as wall-clock
timings rather than LIKWID hardware-counter results. The final `opt3_fast`
executable was compiled with:

```text
-fno-trapping-math -fno-math-errno
```

```text
for x in timestep timestep_opt1 timestep_opt2 timestep_opt3 timestep_opt3_fast; do
    echo "=== $x ==="
    for i in 1 2 3 4 5; do
        /usr/bin/time -f "elapsed=%e s" ./build-linux/$x
    done
done
```
