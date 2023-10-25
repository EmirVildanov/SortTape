cmake_minimum_required(VERSION 3.25)
project(sort_type_core)

set(CMAKE_CXX_STANDARD 17)

set(HEADER_FILES tape.h tests_helpers.h third_party/jsoncpp/dist/json/json.h third_party/jsoncpp/dist/json/json-forwards.h)
set(SOURCE_FILES tape.cpp test_helpers.cpp third_party/jsoncpp/dist/jsoncpp.cpp)

include_directories(third_party/jsoncpp/include)

add_subdirectory(unit_tests)

add_library(sort_tape_lib STATIC ${SOURCE_FILES} ${HEADER_FILES})

#add_subdirectory(third_party/jsoncpp)
#get_target_property(JSON_INC_PATH jsoncpp_lib INTERFACE_INCLUDE_DIRECTORIES)
#include_directories(${JSON_INC_PATH})
#target_link_libraries(sort_tape_lib jsoncpp_lib)