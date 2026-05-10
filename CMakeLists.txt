cmake_minimum_required(VERSION 3.16)
project(playground)

set(CMAKE_C_STANDARD 11)

set(DYNARRAY_DEMO_NAME ${PROJECT_NAME}-dynarray-demo)

add_executable(${DYNARRAY_DEMO_NAME} dynarray/dynarray_demo.c dynarray/dynarray.c)

target_link_libraries(${DYNARRAY_DEMO_NAME} m)
target_compile_options(${DYNARRAY_DEMO_NAME} PRIVATE -Wall -Werror -Wextra)
target_include_directories(${DYNARRAY_DEMO_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${DYNARRAY_DEMO_NAME} PRIVATE DEBUG)
endif()
