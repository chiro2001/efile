cmake_minimum_required(VERSION 3.0.0)
project(efiles-test VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(efiles-test source/test.c source/efiles.c)
add_executable(efiles-simple source/simple.c source/efiles.c)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
