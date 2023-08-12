#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdint.h>

namespace utils
{
    template<typename T, size_t size>
    constexpr size_t array_size(T(&)[size])
    {
        return size;
    }
}

#endif