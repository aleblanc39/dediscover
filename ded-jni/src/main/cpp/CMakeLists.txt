project (jni CXX)
cmake_minimum_required(VERSION 3.0)

find_package(JNI)

include (${XDE_CPP_DIR}/xde-config.cmake)

message("XDE_CPP_DIR: " ${XDE_CPP_DIR})

message("JNI_PROJECT_ROOT: " ${JNI_PROJECT_ROOT})
message("JNI include file: " ${JAVA_INCLUDE_PATH} " "  ${JAVA_INCLUDE_PATH2} )
message("Current dir: " ${CMAKE_CURRENT_SOURCE_DIR})


set (CURRENT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
file (GLOB_RECURSE  SRC_FILES *.cc)

find_library(MUPARSER NAMES  muparser  PATHS ${XDE_CPP_DIR}/build/cmake/muparser)

message(${XDE_CPP_DIR})
find_library(SUNDIALS_CVODE sundials_cvode  PATHS ${XDE_CPP_DIR}/build/cmake/cvode/src/cvode/)
find_library(SUNDIALS_NVECSERIAL sundials_nvecserial  PATHS ${XDE_CPP_DIR}/build/cmake/cvode/src/nvector/serial/)

find_library(XDE xde PATHS ${XDE_CPP_DIR}/build/cmake/xde)

# TODO Need a better way to specify the include files from cvode build...
# TODO consolidate. Maybe mode these paths to the xde-config files?
include_directories(${JAVA_INCLUDE_PATH} 
      ${JAVA_INCLUDE_PATH2}
      ${JNI_PROJECT_ROOT}/build/generated/headers
      ${JNI_PROJECT_ROOT}/src/main/cpp/src 
      ${XDE_CPP_DIR}/xde
      ${XDE_CPP_DIR}/SplineLib/src/baf
      ${XDE_CPP_DIR}/SplineLib/src/spl
      ${XDE_CPP_DIR}/SplineLib/src/util

      ${XDE_CPP_DIR}/muparser/include 
      ${XDE_CPP_DIR}/cvode/include
      ${PROJECT_BINARY_DIR}/../../ded-cpp/build/cvode/include
      ${PROJECT_BINARY_DIR}/../../../ded-cpp/build/cmake/cvode/include
      ${JNI_PROJECT_ROOT}/../ded-cpp/build/cmake/cvode/include
 

      )

set_source_files_properties(${SRC_FILES}     
      PROPERTIES 
      COMPILE_FLAGS  "${EXTRA_OPTIONS}")

# This is always going to be a shared library.
add_library(DEDiscover SHARED ${SRC_FILES})

# Need to find a way to alias all the boost libraries.
target_link_libraries(DEDiscover ${EXTRA_OPTIONS} ${SUNDIALS_CVODE} ${SUNDIALS_NVECSERIAL}
  ${MUPARSER} ${XDE} ${BOOST_LIBRARIES}  ${XERCES})

message("Done with cmake.")
