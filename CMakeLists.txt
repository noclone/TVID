cmake_minimum_required(VERSION 3.10)
project(TVID C)

set(CMAKE_C_STANDARD 17)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

set(SOURCE_FILES
        src/main.c
        src/format.c
        src/colors.c
        src/parser.c
        src/display.c
        src/io.c
        src/header.c
        src/bob.c
)

add_executable(TVID ${SOURCE_FILES})

target_link_libraries(TVID ${SDL2_LIBRARIES} ${SDL2IMAGE_LIBRARIES})
