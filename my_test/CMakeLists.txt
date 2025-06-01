find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(input_test
        ../src/system/input.cpp
        input_test.cpp
)

add_executable(train_system_test
        ../src/buffer/lru_k_replacer.cpp
        ../src/buffer/disk_manager.cpp
        ../src/buffer/buffer_pool_manager.cpp
        ../src/b_plus_tree/page_guard.cpp
        ../src/b_plus_tree/b_plus_tree_page.cpp
        ../src/b_plus_tree/b_plus_tree_leaf_page.cpp
        ../src/b_plus_tree/b_plus_tree_internal_page.cpp
        ../src/b_plus_tree/b_plus_tree.cpp
        ../src/system/input.cpp
        ../src/system/user_system/user_system.cpp
        ../src/system/train_system/train_system.cpp
        ../src/system/ticket_system/ticket_system.cpp
        ../src/system/system.cpp
        train_system_test.cpp)

add_executable(ticket_system_test
        ../src/buffer/lru_k_replacer.cpp
        ../src/buffer/disk_manager.cpp
        ../src/buffer/buffer_pool_manager.cpp
        ../src/b_plus_tree/page_guard.cpp
        ../src/b_plus_tree/b_plus_tree_page.cpp
        ../src/b_plus_tree/b_plus_tree_leaf_page.cpp
        ../src/b_plus_tree/b_plus_tree_internal_page.cpp
        ../src/b_plus_tree/b_plus_tree.cpp
        ../src/system/input.cpp
        ../src/system/user_system/user_system.cpp
        ../src/system/train_system/train_system.cpp
        ../src/system/ticket_system/ticket_system.cpp
        ../src/system/system.cpp
        ticket_system_test.cpp)

target_link_libraries(input_test ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})

target_link_libraries(train_system_test ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})

target_link_libraries(ticket_system_test ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})

add_test(NAME input_test COMMAND input_test)

add_test(NAME train_system_test COMMAND train_system_test)

add_test(NAME ticket_system_test COMMAND ticket_system_test)