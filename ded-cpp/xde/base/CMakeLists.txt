project (xde_base CXX)
cmake_minimum_required(VERSION 3.0)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".so.0" ".so.1" ".so.2" ".so.3" ".so.4" ".so.5" ".so.1.62.0")
include ("../../xde-config.cmake")
message("bin dir for base: ${PROJECT_BINARY_DIR}")


set (SUNDIALS_CVODE sundials_cvode)
set (SUNDIALS_NVECSERIAL sundials_nvecserial)
set (XDEFORTRAN  xde_fortran)
set (MUPARSER  muparser)
set (SPLINELIB splinelibspl)
set (SPLINEBAF splinelibbaf)

find_library(GFORTRAN gfortran PATHS /usr/lib/gcc/x86_64-linux-gnu/8 /usr/lib64 /usr/lib/gcc/x86_64-linux-gnu/9/)
find_library(LAPACK  NAMES lapack  PATHS /usr/lib/x86_64-linux-gnu/ /usr/lib64 /usr/lib)
find_library(BLAS NAMES blas PATHS /usr/lib/x86_64-linux-gnu/ /usr/lib /usr/lib64)

set (sources 

./XDECSVFile.cc
./MemoryBank.cc
./XDEUtil.cc
./XDEMessage.cc
./ThreadInterface.cc
./XDEMessage.cc

)

set_source_files_properties(${sources}
  PROPERTIES 
  COMPILE_FLAGS  "${EXTRA_OPTIONS}")


include_directories(
  ..
#   ../muparser/include
#   ../cvode/include
#   ../f90/include
#   ../SplineLib/src/baf
#   ../SplineLib/src/spl
#   ../SplineLib/src/util

#   ${PROJECT_BINARY_DIR}/../cvode/include
)

link_directories("${PROJECT_BINARY_DIR}/../cvode/src/cvode")
link_directories("${PROJECT_BINARY_DIR}/../cvode/src/nvec_ser")
link_directories("${PROJECT_BINARY_DIR}/../SplineLib/src/spl")
link_directories("${PROJECT_BINARY_DIR}/../SplineLib/src/baf")


add_library(xde_base  ${LIBRARY_TYPE} ${sources} )

target_link_libraries(xde_base ${EXTRA_OPTIONS}
#   ${SPLINELIB}
#   ${SPLINEBAF}
#   ${SUNDIALS_CVODE}
#   ${SUNDIALS_NVECSERIAL}
#   ${XDEFORTRAN}
#   ${BLAS}
#   ${LAPACK}
#   ${GFORTRAN}
#   ${MUPARSER}
  ${BOOST_LIBRARIES}
#  ${CMATH}
)
message("Done with cmake.")