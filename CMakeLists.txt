cmake_minimum_required(VERSION 3.2)
project(lessdb)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Installs boost library
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(CXX_FLAGS
        -Wall
        -Wold-style-cast #warn if C-style implicit-casting occurs
        -Wconversion
        )
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

set(LESSDB_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)