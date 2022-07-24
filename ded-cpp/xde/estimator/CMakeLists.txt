project (xde_estimator CXX)
cmake_minimum_required(VERSION 3.0)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".so.0" ".so.1" ".so.2" ".so.3" ".so.4" ".so.5" ".so.1.62.0")
include ("../../xde-config.cmake")
message("bin dir for xde_estimator: ${PROJECT_BINARY_DIR}")

set (sources 

./SolverBasedEstimator.cc
./XDEEstimator.cc
)

set_source_files_properties(${sources}
  PROPERTIES 
  COMPILE_FLAGS  "${EXTRA_OPTIONS}")


include_directories(
  ..
  ../../cvode/include
  ${PROJECT_BINARY_DIR}/../../cvode/include
  ../../muparser/include
)

find_library(XDE_BASE xde_base ${PROJECT_BINARY_DIR}/../base)
add_library(xde_estimator  ${LIBRARY_TYPE} ${sources} )

target_link_libraries(xde_estimator ${EXTRA_OPTIONS}
  ${BOOST_LIBRARIES}
  ${XDE_BASE}
)
message("Done with xde_estimator cmake.")