enable_testing()

find_package(GTest REQUIRED)

# 单元测试统计覆盖率
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")

set(LINK_LIBS GTest::GTest GTest::Main)

file(GLOB DEMO_TEST_SRC ./*.cpp)

add_executable(demo-test ${DEMO_TEST_SRC})

target_link_libraries(demo-test PRIVATE ${LINK_LIBS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_test(demo-test demo-test)
