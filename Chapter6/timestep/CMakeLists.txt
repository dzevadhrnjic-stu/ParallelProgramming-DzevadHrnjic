cmake_minimum_required(VERSION 3.5)
project(timestep C)

set(CMAKE_C_STANDARD 99)
set(VECTOR_WIDTH 256 CACHE STRING "Preferred SIMD vector width in bits")

set(TIMESTEP_C_FLAGS
    -g
    -O3
    -fstrict-aliasing
    -ftree-vectorize
    -fopenmp-simd
    -march=native
    -mtune=native
    -mprefer-vector-width=${VECTOR_WIDTH}
    -fopt-info-vec-optimized
    -fopt-info-vec-missed
)

if(CMAKE_C_COMPILER_ID STREQUAL "Clang")
    list(APPEND TIMESTEP_C_FLAGS
        -Rpass=loop-vectorize
        -Rpass-analysis=loop-vectorize
    )
elseif(CMAKE_C_COMPILER_ID STREQUAL "Intel")
    list(APPEND TIMESTEP_C_FLAGS
        -qopt-report=5
        -qopt-report-phase=vec,loop
    )
endif()

add_compile_options(${TIMESTEP_C_FLAGS})

add_executable(timestep main.c timestep.c)
target_link_libraries(timestep m)

add_executable(timestep_opt1 main.c timestep_opt1.c)
target_link_libraries(timestep_opt1 m)

add_executable(timestep_opt2 main.c timestep_opt2.c)
target_link_libraries(timestep_opt2 m)

add_executable(timestep_opt3 main.c timestep_opt3.c)
target_link_libraries(timestep_opt3 m)

add_executable(timestep_opt3_fast main.c timestep_opt3.c)
target_compile_options(timestep_opt3_fast PRIVATE
    -fno-trapping-math
    -fno-math-errno
)
target_link_libraries(timestep_opt3_fast m)

add_custom_target(distclean
    COMMAND ${CMAKE_COMMAND} -E rm -rf
        CMakeCache.txt
        CMakeFiles
        Makefile
        cmake_install.cmake
)
