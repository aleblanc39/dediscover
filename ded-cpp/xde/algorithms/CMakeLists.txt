project (xde_algorithms CXX)
cmake_minimum_required(VERSION 3.0)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".so.0" ".so.1" ".so.2" ".so.3" ".so.4" ".so.5" ".so.1.62.0")
include ("../../xde-config.cmake")
message("bin dir for xde_algorithms: ${PROJECT_BINARY_DIR}")

set (sources 

./GeneralParameter.cc
./GenericParameter.cc
./AlgorithmTemplates.cc
)

set_source_files_properties(${sources}
  PROPERTIES 
  COMPILE_FLAGS  "${EXTRA_OPTIONS}")


include_directories(
  ..

)

find_library(XDE_BASE xde_base ${PROJECT_BINARY_DIR}/../base)
add_library(xde_algorithms  ${LIBRARY_TYPE} ${sources} )

target_link_libraries(xde_algorithms ${EXTRA_OPTIONS}
  ${BOOST_LIBRARIES}
  ${XDE_BASE}
)
message("Done with xde_algorithms cmake.")