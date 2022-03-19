cmake_minimum_required(VERSION 3.4.1)
project(os-course-design)
add_compile_options(-g -std=c++14)
add_executable(my-os main.cpp storage.cpp inode.cpp)
