project(unit_tests)
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(unit_tests_run SortTapeTest.cpp)
target_link_libraries(unit_tests_run sort_tape_lib)
target_link_libraries(unit_tests_run gtest gtest_main)