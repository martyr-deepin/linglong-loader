cmake_minimum_required(VERSION 3.10)

project(linglong_load)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)

# configure debug mode and asan
#if (NOT CMAKE_BUILD_TYPE)
#    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose Release or Debug" FORCE)
#endif ()

# debug mode
#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#    ADD_DEFINITIONS(-DDEBUG)
#endif ()

link_libraries(stdc++fs)

# thread --> tasn ; memory --> asan ; default --> none
#if (NOT CMAKE_DEBUG_SANIT)
#    set(CMAKE_DEBUG_SANIT "none" CACHE STRING "Choose [none,thread,memory]" FORCE)
#endif()
#if (CMAKE_BUILD_TYPE STREQUAL "Debug")
#    message("STATUS" "Compiler Feature: " ${CMAKE_DEBUG_SANIT})
#    if(CMAKE_DEBUG_SANIT STREQUAL "thread")
#        set(CMAKE_CXX_FLAGS_RELEASE "")
#        set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fsanitize=thread -Wall -g -ggdb3")
#        set(CMAKE_CXX_FLAGS "-O0 -fsanitize=thread -Wall -g -ggdb3")
#    elseif(CMAKE_DEBUG_SANIT STREQUAL "memory")
#        set(CMAKE_CXX_FLAGS_RELEASE "")
#        set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fsanitize=undefined,address -Wall -g -ggdb3")
#        set(CMAKE_CXX_FLAGS "-O0 -fsanitize=undefined,address -Wall -g -ggdb3")
#    elseif(CMAKE_DEBUG_SANIT STREQUAL "none")
#        set(CMAKE_CXX_FLAGS_RELEASE "")
#        set(CMAKE_CXX_FLAGS_DEBUG "-O0  -Wall -g -ggdb3")
#        set(CMAKE_CXX_FLAGS "-O0 -Wall -g -ggdb3")
#    else()
#        set(CMAKE_CXX_FLAGS_RELEASE "")
#        set(CMAKE_CXX_FLAGS_DEBUG "-O0  -Wall -g -ggdb3")
#        set(CMAKE_CXX_FLAGS "-O0 -Wall -g -ggdb3")
#    endif()
#endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3  -Wall")
set(CMAKE_CXX_FLAGS_DEBUG "-O3  -Wall")
set(CMAKE_CXX_FLAGS "-O3  -Wall")

add_subdirectory(src)

add_subdirectory(tests)
