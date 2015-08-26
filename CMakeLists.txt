cmake_minimum_required(VERSION 3.2)
project(lessdb)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Installs boost library
find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# header search path
include_directories(
        util
)

set(LESSDB_SRC
        lessdb.h lessdb.cc
)

set(LESSDB_UTIL
        status_test.cc status.h
)

add_executable(status status.h status_test.cc)

add_executable(sds sds.cc sds.h sds_test.cc)