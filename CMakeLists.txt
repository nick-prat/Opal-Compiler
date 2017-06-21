cmake_minimum_required(VERSION 3.5)
project(Opal-Compiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -Wextra -pedantic -std=c++1z")
set(CMAKE_CXX_COMPILER /usr/bin/clang++)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DURECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/release)

set(SOURCE_FILES
    src/main.cc
    src/resources.cc
    src/util.cc
    src/scenehandler.cc
)

add_executable(oplc ${SOURCE_FILES})

find_package(ASSIMP REQUIRED)
include_directories(${ASSIMP_INCLUDE_DIR} "src/")
target_link_libraries(oplc /usr/lib/libfreeimage.so ${ASSIMP_LIBRARIES})
