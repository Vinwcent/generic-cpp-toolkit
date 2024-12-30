cmake_minimum_required(VERSION 3.14)
project(CMakeProject)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CXX_STANDARD_REQUIRED ON)

set(CMAKE_BUILD_TYPE Debug)
find_package(GTest REQUIRED)

# Lib
set(GEN_LIB_FILES)
list(APPEND GEN_LIB_FILES
	src/priority_event_queue.hpp
	src/function_wrappers.hpp
	src/args_storage.hpp
)
add_library(GenLib INTERFACE ${GEN_LIB_FILES})

# Tests
enable_testing()
include(GoogleTest)
set(TESTS_FILES)
list(APPEND TESTS_FILES
	tests/priority_event_queue_test.cpp
	tests/function_wrappers_test.cpp
	tests/args_storage_test.cpp
)

set(TESTS_EXECUTABLE tests)
add_executable(${TESTS_EXECUTABLE} ${TESTS_FILES})
target_include_directories(${TESTS_EXECUTABLE} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
	${GTEST_INCLUDE_DIRS})
target_link_libraries(${TESTS_EXECUTABLE} GTest::gtest_main GTest::gtest)
gtest_discover_tests(
	${TESTS_EXECUTABLE}
)
