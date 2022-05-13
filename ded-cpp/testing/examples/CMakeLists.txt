project (xde_examples CXX)

cmake_minimum_required(VERSION 3.0)


include(${XDE_CPP_DIR}/xde-config.cmake)
file (GLOB_RECURSE  SRC_FILES "*_ex.cc")

include_directories(
  ${XDE_CPP_DIR}/testing/xde-tests/test-utils
  ./test-utils
  ${XDE_CPP_DIR}/xde
  ${XDE_CPP_DIR}/f90/include
  ${XDE_CPP_DIR}/muparser/include
  ${XDE_CPP_DIR}/cvode/include
)

# Not needed for other libraries (mathstat, muparser). Maybe because they are not
# directly under build/

link_directories("${PROJECT_BINARY_DIR}/../../cvode/src/cvode")
link_directories("${PROJECT_BINARY_DIR}/../../cvode/src/nvec_ser")
link_directories("${PROJECT_BINARY_DIR}/../../SplineLib/src/spl")
link_directories("${PROJECT_BINARY_DIR}/../../SplineLib/src/baf")

find_library(PTHREAD pthread /usr/lib/x86_64-linux-gnu/)
#find_library(XDE xde PATHS ${XDE_CPP_DIR}/build/cmake/xde)

add_compile_options("-DLINUX")

foreach (i ${SRC_FILES})
    set_source_files_properties(${i} 
        PROPERTIES
        COMPILE_FLAGS  "${EXTRA_OPTIONS} ")

    get_filename_component(executable ${i} NAME_WE )
    add_executable(${executable}  ${i})
    target_link_libraries(${executable} xde  ${PTHREAD} ${BOOST_LIBRARIES})
endforeach (i ${SRC_FILES})
