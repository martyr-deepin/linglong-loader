cmake_minimum_required(VERSION 3.10)

project(linglong_load)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)

link_libraries(stdc++fs)

set(CMAKE_CXX_FLAGS_RELEASE "-O3  -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O3  -Wall")
set(CMAKE_CXX_FLAGS "-O3  -Wall")

add_subdirectory(src)