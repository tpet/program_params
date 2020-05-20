cmake_minimum_required(VERSION 3.0)
project(program_params)

add_compile_options(-std=c++11)

include_directories(include)

add_executable(overview examples/overview.cpp)
add_executable(values examples/values.cpp)
