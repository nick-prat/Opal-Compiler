cmake_minimum_required(VERSION 3.5)
project(Opal-Compiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Wextra -pedantic -std=c++1z -fcolor-diagnostics")
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DURECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)

set(SOURCE_FILES
    client/main.cc
)

add_executable(oplc ${SOURCE_FILES})

find_package(ASSIMP REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
add_definitions(${GTK3_CFLAGS_OTHER})

include_directories(${ASSIMP_INCLUDE_DIR} ${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
target_link_libraries(oplc /usr/lib/libfreeimage.so ${ASSIMP_LIBRARIES} ${GTK3_LIBRARIES} /usr/local/lib/libopal.so)
