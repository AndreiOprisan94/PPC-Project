cmake_minimum_required(VERSION 3.10)
project(ParallelMatrixMultiplication)

set(CMAKE_CXX_STANDARD 11)
include_directories($ENV{MPIROOT}/include)

add_executable(ParallelMatrixMultiplication main.cpp)