cmake_minimum_required(VERSION 3.25)
project(sort_tape)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wextra-semi -O1 -g -fsanitize=address -fno-omit-frame-pointer")

set(SOURCE_FILES main.cpp)
add_executable(sort_tape ${SOURCE_FILES})

include_directories(lib)
add_subdirectory(lib)

target_link_libraries(sort_tape sort_tape_lib)

set_target_properties(sort_tape PROPERTIES LINKER_LANGUAGE CXX)