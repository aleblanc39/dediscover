cmake_minimum_required(VERSION 3.0.0)
project(ded-cpp VERSION 0.1.0)

include(CTest)
enable_testing()

add_subdirectory(cvode)
add_subdirectory(muparser)
add_subdirectory(f90)
add_subdirectory(SplineLib)
add_subdirectory(xde)
add_subdirectory(testing/xde-tests)
add_subdirectory(testing/examples)


add_dependencies(xde  sundials_cvode_shared sundials_nvecserial_shared muparser xde_fortran splinelibspl)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
