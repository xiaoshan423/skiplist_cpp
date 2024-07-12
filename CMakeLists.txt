cmake_minimum_required(VERSION 3.8)

project(mySkipList)

add_compile_options(--std=c++11 -O3)

include_directories(
    include
)

find_package(Threads REQUIRED)

add_executable(${PROJECT_NAME} src/main.cpp)

add_executable(stress-test stress-test/stress_test.cpp)

target_link_libraries(stress-test ${CMAKE_THREAD_LIBS_INIT})
