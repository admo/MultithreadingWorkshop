cmake_minimum_required(VERSION 3.5)
project(thread_workshop)

set(CMAKE_CXX_STANDARD 17)

find_package(Threads)

add_executable(thread_workshop main.cpp barrier.h)
target_link_libraries(thread_workshop ${CMAKE_THREAD_LIBS_INIT})