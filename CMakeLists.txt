cmake_minimum_required(VERSION 3.10)
project(TVID C)

set(CMAKE_C_STANDARD 17)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
pkg_check_modules(GDKPIXBUF REQUIRED gdk-pixbuf-2.0)

set(SOURCE_FILES
    src/main.c
    src/format.c
    src/colors.c
    src/parser.c
    src/display.c
    src/io.c
)

add_executable(TVID ${SOURCE_FILES})

include_directories(${GTK3_INCLUDE_DIRS} ${GDKPIXBUF_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS} ${GDKPIXBUF_LIBRARY_DIRS})

add_definitions(${GTK3_CFLAGS_OTHER})

target_link_libraries(TVID ${GTK3_LIBRARIES})
