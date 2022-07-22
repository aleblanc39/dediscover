project (xde_base_tests CXX)

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

# Not needed for other libraries (mathstat, muparser). Maybe because they are not
# directly under build/
# message("bin dir fot test: ${PROJECT_BINARY_DIR}")
# link_directories("${PROJECT_BINARY_DIR}/../../cvode/src/cvode")
# link_directories("${PROJECT_BINARY_DIR}/../../cvode/src/nvec_ser")
# link_directories("${PROJECT_BINARY_DIR}/../../SplineLib/src/spl")

# link_directories("${PROJECT_BINARY_DIR}/../../SplineLib/src/baf")



# find_library(PTHREAD pthread /usr/lib/x86_64-linux-gnu/)

#add_compile_options("-DLINUX -DBOOST_MATH_NO_LONG_DOUBLE_MATH_FUNCTIONS")
# Unit Testing


#add_executable(ModelTest model/ModelTest.cc $<TARGET_OBJECTS:test_main>)


#target_link_libraries(ModelTest ${XDE}  ${PTHREAD} ${BOOST_LIBRARIES})
#add_test(NAME ModelTest COMMAND ModelTest)



add_library(test_main OBJECT ../test-utils/test_main.cc )

message("Build dir: " ${BUILD_DIR})
find_library(XDE_BASE NAMES xde_base PATHS ${BUILD_DIR})
foreach (i ${SRC_FILES})
    set_source_files_properties(${i} 
        PROPERTIES
        COMPILE_FLAGS  "${EXTRA_OPTIONS} ")


    get_filename_component(executable ${i} NAME_WE )
    add_executable(${executable}  ${i} $<TARGET_OBJECTS:test_main>)
    target_link_libraries(${executable} ${XDE_BASE}   ${BOOST_LIBRARIES})
    add_test(NAME ${executable} COMMAND ${executable})
endforeach (i ${SRC_FILES})
