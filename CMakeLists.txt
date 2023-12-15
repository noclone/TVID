cmake_minimum_required(VERSION 3.27)
project(TVID C)

set(CMAKE_C_STANDARD 17)

add_executable(TVID src/main.c src/format.c src/colors.c)
