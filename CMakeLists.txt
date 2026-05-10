cmake_minimum_required(VERSION 3.16)
project(playground)

set(CMAKE_C_STANDARD 11)

set(TESTING_DEMO_NAME ${PROJECT_NAME}-testing-demo)
add_executable(${TESTING_DEMO_NAME}
        testing/testing_demo.c)
target_link_libraries(${TESTING_DEMO_NAME} m)
target_compile_options(${TESTING_DEMO_NAME} PRIVATE -Wall -Werror -Wextra -Wpointer-arith)
target_include_directories(${TESTING_DEMO_NAME} PRIVATE ${CMAKE_SOURCE_DIR})
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${TESTING_DEMO_NAME} PRIVATE DEBUG)
endif()

set(DYNAMIC_ARRAY_TEST_NAME ${PROJECT_NAME}-vector)
add_executable(${DYNAMIC_ARRAY_TEST_NAME}
        collections/vector_test.c)
target_link_libraries(${DYNAMIC_ARRAY_TEST_NAME} m)
target_compile_options(${DYNAMIC_ARRAY_TEST_NAME} PRIVATE -Wall -Werror -Wextra -Wpointer-arith)
target_include_directories(${DYNAMIC_ARRAY_TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/collections)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${DYNAMIC_ARRAY_TEST_NAME} PRIVATE DEBUG)
endif()

set(MAP_TEST_NAME ${PROJECT_NAME}-map)
add_executable(${MAP_TEST_NAME}
        collections/map.c)
target_link_libraries(${MAP_TEST_NAME} m)
target_compile_options(${MAP_TEST_NAME} PRIVATE -Wall -Werror -Wextra -Wpointer-arith)
target_include_directories(${MAP_TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/collections)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${MAP_TEST_NAME} PRIVATE DEBUG)
endif()

set(LINKEDLIST_TEST_NAME ${PROJECT_NAME}-list)
add_executable(${LINKEDLIST_TEST_NAME}
        collections/list_test.c)
target_link_libraries(${LINKEDLIST_TEST_NAME} m)
target_compile_options(${LINKEDLIST_TEST_NAME} PRIVATE -Wall -Werror -Wextra -Wpointer-arith)
target_include_directories(${LINKEDLIST_TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/collections)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${LINKEDLIST_TEST_NAME} PRIVATE DEBUG)
endif()