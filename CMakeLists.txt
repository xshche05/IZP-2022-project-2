cmake_minimum_required(VERSION 3.23)
project(cluster C)

set(CMAKE_C_STANDARD 99)

add_compile_options(-lm)
add_compile_options(-Wall -Wextra -Werror)

add_executable(cluster main.c)
