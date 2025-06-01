# Ticket System

author : sword

finish debugging at 2025.6.1 (Happy Children's Day!)

## File Strucure

```
src
├── b_plus_tree
│   ├── b_plus_tree.cpp
│   ├── b_plus_tree_internal_page.cpp
│   ├── b_plus_tree_leaf_page.cpp
│   ├── b_plus_tree_page.cpp
│   ├── page_guard.cpp
│   └── run.cpp
├── buffer
│   ├── buffer_pool_manager.cpp
│   ├── disk_manager.cpp
│   └── lru_k_replacer.cpp
├── include
│   ├── b_plus_tree
│   │   ├── b_plus_tree.h
│   │   ├── b_plus_tree_header_page.h
│   │   ├── b_plus_tree_internal_page.h
│   │   ├── b_plus_tree_leaf_page.h
│   │   ├── b_plus_tree_page.h
│   │   └── page_guard.h
│   ├── buffer
│   │   ├── buffer_pool_manager.h
│   │   ├── disk_manager.h
│   │   └── lru_k_replacer.h
│   ├── comparator.h
│   ├── config.h
│   ├── memory_river
│   │   └── memory_river.hpp
│   ├── my_stl
│   │   ├── algorithm.hpp
│   │   ├── array.hpp
│   │   ├── exceptions.hpp
│   │   ├── list.hpp
│   │   ├── map.hpp
│   │   ├── utility.hpp
│   │   └── vector.hpp
│   └── system
│       ├── input.h
│       ├── output.hpp
│       ├── system.h
│       ├── ticket_system
│       │   ├── ticket.h
│       │   └── ticket_system.h
│       ├── train_system
│       │   ├── train.h
│       │   └── train_system.h
│       └── user_system
│           ├── user.h
│           └── user_system.h
├── main.cpp
└── system
    ├── input.cpp
    ├── system.cpp
    ├── ticket_system
    │   └── ticket_system.cpp
    ├── train_system
    │   └── train_system.cpp
    └── user_system
        └── user_system.cpp
```

## Acknowledgement

1. livandan
2. Vitalrubbish
3. Zihan Zhang TA
4. zxsheather