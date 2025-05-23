find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(input_test
        ../src/system/input.cpp
        input_test.cpp
)

target_link_libraries(input_test ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})

add_test(NAME input_test COMMAND input_test)