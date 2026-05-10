cmake_minimum_required(VERSION 3.16)
project(playground)

set(CMAKE_C_STANDARD 11)

set(VECTOR_TEST_NAME ${PROJECT_NAME}-vector-test)
add_executable(${VECTOR_TEST_NAME}
        collections/vector/vector.c
        collections/vector/test_vector.c)
target_link_libraries(${VECTOR_TEST_NAME} m)
target_compile_options(${VECTOR_TEST_NAME} PRIVATE -Wall -Werror -Wextra -Wpointer-arith)
target_include_directories(${VECTOR_TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${VECTOR_TEST_NAME} PRIVATE DEBUG)
endif()

set(HASHTABLE_TEST_NAME ${PROJECT_NAME}-hashtable-test)
add_executable(${HASHTABLE_TEST_NAME}
        collections/hashtable/hashtable.c
        collections/hashtable/test_hashtable.c)
target_link_libraries(${HASHTABLE_TEST_NAME} m)
target_compile_options(${HASHTABLE_TEST_NAME} PRIVATE -Wall -Werror -Wextra -Wpointer-arith)
target_include_directories(${HASHTABLE_TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${HASHTABLE_TEST_NAME} PRIVATE DEBUG)
endif()