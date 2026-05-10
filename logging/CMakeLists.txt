cmake_minimum_required(VERSION 3.16)
project(logging-demo)

set(CMAKE_C_STANDARD 11)

add_executable(${PROJECT_NAME} logging_demo.c)
target_link_libraries(${PROJECT_NAME} m)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Werror -Wextra -Wpointer-arith)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()