#pragma once

// Some macros used throughout the project.

#if defined _TREE_DEBUG && _TREE_DEBUG > 0

    #include <iostream>

    #define LOG(msg) std::cout << msg << std::endl

    #define IS_NULL(ptr) ((ptr) == 0 ? "null" : "not null")

#else

    #define LOG(msg)

    #define IS_NULL(ptr)

#endif

#if defined _TREE_DEBUG && _TREE_DEBUG > 1

    #define LOGV(msg) std::cout << "[ in " << __func__ << " ]: " << msg << std::endl

#else

    #define LOGV(msg)

#endif