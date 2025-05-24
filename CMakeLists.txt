cmake_minimum_required(VERSION 3.22)
project(TicketSystem)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")

include_directories(src/include)

add_subdirectory(my_test)

add_executable(code
        src/buffer/lru_k_replacer.cpp
        src/buffer/disk_manager.cpp
        src/buffer/buffer_pool_manager.cpp
        src/b_plus_tree/page_guard.cpp
        src/b_plus_tree/b_plus_tree_page.cpp
        src/b_plus_tree/b_plus_tree_leaf_page.cpp
        src/b_plus_tree/b_plus_tree_internal_page.cpp
        src/b_plus_tree/b_plus_tree.cpp
        src/system/input.cpp
        src/system/user_system/user_system.cpp
        src/system/system.cpp
        src/main.cpp)