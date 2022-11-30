set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")

set(EXENAME "linglong-loader")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(${EXENAME} elf.cpp erofs.cpp main.cpp)

link_libraries("-static")

target_include_directories(${EXENAME} PRIVATE)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SELINUX REQUIRED libselinux)

set(LINK_LIBS
    ${SELINUX_STATIC_LIBRARIES}
    pthread
    erofs
    erofsfuse
    lz4
    lzma
    lzo2
    z
    fuse
    stdc++
    zstd
    dl)

target_link_libraries(${EXENAME} erofsfuse ${LINK_LIBS})

install(TARGETS linglong-loader
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/libexec)
