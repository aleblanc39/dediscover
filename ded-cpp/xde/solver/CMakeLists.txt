project (xde_solver CXX)
cmake_minimum_required(VERSION 3.0)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".so.0" ".so.1" ".so.2" ".so.3" ".so.4" ".so.5" ".so.1.62.0")
include ("../../xde-config.cmake")
message("bin dir for xde_algorithms: ${PROJECT_BINARY_DIR}")

set (SUNDIALS_CVODE sundials_cvode)


set (sources 
./SolverResult.cc
./SundialsSolverWrapper.cc
./SambineDDEWrapper.cc
./XDESolverWrapper.cc

)

set_source_files_properties(${sources}
  PROPERTIES 
  COMPILE_FLAGS  "${EXTRA_OPTIONS}")


include_directories(
  ..
  ../../cvode/include
  ../../f90/include
  ${PROJECT_BINARY_DIR}/../../cvode/include
  ../../muparser/include
)

find_library(XDE_ALGORITHMS xde_algorithms ${PROJECT_BINARY_DIR}/../algorithms)
add_library(xde_solver  ${LIBRARY_TYPE} ${sources} )

target_link_libraries(xde_solver ${EXTRA_OPTIONS}
  ${BOOST_LIBRARIES}
  ${XDE_ALGORITHMS}
)
message("Done with xde_solver cmake.")