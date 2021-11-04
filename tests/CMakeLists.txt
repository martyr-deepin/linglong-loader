enable_testing()

find_package(GTest REQUIRED)

set(LINK_LIBS GTest::GTest GTest::Main)

file(GLOB DEMO_TEST_SRC ./*.cpp)

add_executable(demo-test ${DEMO_TEST_SRC})

target_link_libraries(demo-test PRIVATE ${LINK_LIBS})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_test(demo-test demo-test)