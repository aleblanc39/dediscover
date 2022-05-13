project (xde_fortran Fortran)

cmake_minimum_required(VERSION 3.0)
find_library(BLAS blas /usr/lib/x86_64-linux-gnu/ /usr/lib /usr/lib64 )
find_library(LAPACK lapack /usr/lib /usr/lib64 /usr/lib/x86_64-linux-gnu/)

set (CURRENT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file (GLOB_RECURSE  SRC_FILES *.f90 )

set(FORTRAN_OPTIONS "-fno-range-check -O3   -fbacktrace -fPIC")



set_source_files_properties(${SRC_FILES} PROPERTIES COMPILE_FLAGS "${FORTRAN_OPTIONS}")

add_library(xde_fortran SHARED ${SRC_FILES})


target_link_libraries(xde_fortran
  ${BLAS}
  ${LAPACK}
 )
