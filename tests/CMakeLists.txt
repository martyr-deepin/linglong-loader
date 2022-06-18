enable_testing()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")

find_package(GTest REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core DBus)

# 单元测试统计覆盖率
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")

set(Qt_LIBS Qt5::Core Qt5::DBus)

set(LINK_LIBS squashfuse_ll
    squashfuse
    fuseprivate
    lz4
    lzma
    lzo2
    z
    fuse
    stdc++
    pthread
    zstd
    dl
    GTest::GTest GTest::Main)

set(GTEST_SOURCES
    ${PROJECT_SOURCE_DIR}/src/squashfs.cpp
    ${PROJECT_SOURCE_DIR}/src/elf.cpp
    demo_test.cpp)

add_executable(demo-test ${GTEST_SOURCES})

target_link_libraries(demo-test PRIVATE ${LINK_LIBS} ${Qt_LIBS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_test(demo-test demo-test)
