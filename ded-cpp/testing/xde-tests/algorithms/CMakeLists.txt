project (xde_algorithms_tests CXX)

cmake_minimum_required(VERSION 3.0)

include(CTest)
enable_testing()

include(${XDE_CPP_DIR}/xde-config.cmake)
message("XDE_CPP_DIR: "  ${XDE_CPP_DIR})
file (GLOB SRC_FILES "*Test.cc")

include_directories(
  ${XDE_CPP_DIR}/testing/xde-tests/test-utils
  ../test-utils
  ${XDE_CPP_DIR}/xde
)

add_library(test_main OBJECT ../test-utils/test_main.cc )

message("Build dir: " ${BUILD_DIR})
find_library(XDE_ALGORITHMS NAMES xde_algorithms PATHS ${BUILD_DIR})
find_library(XDE_BASE NAMES xde_base PATHS ${BUILD_DIR}/../base)

foreach (i ${SRC_FILES})
    set_source_files_properties(${i} 
        PROPERTIES
        COMPILE_FLAGS  "${EXTRA_OPTIONS} ")

    get_filename_component(executable ${i} NAME_WE )
    add_executable(${executable}  ${i} $<TARGET_OBJECTS:test_main>)
    target_link_libraries(${executable} ${XDE_ALGORITHMS}   ${BOOST_LIBRARIES} ${XDE_BASE})
    add_test(NAME ${executable} COMMAND ${executable})
endforeach (i ${SRC_FILES})
