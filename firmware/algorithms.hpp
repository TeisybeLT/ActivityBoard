#ifndef ALGORITHMS_HPP
#define ALGORITHMS_HPP

#include "traits.hpp"
#include <stddef.h>

namespace algorithms
{
    template<typename T>
    [[nodiscard]] constexpr T* midpoint(T* a, T* b)
    {
        return a + (b - a) / 2;
    }

    template<traits::signed_integral T>
    [[nodiscard]] constexpr const T* binary_interval_search(const T* a, const T* b, T value)
    {
        if (b - a == 2)
            return ((value - *a) > (*(b-1) - value)) ? b - 1 : a;

        auto* mid = midpoint(a, b);

        if (*mid > value)
            return binary_interval_search(a, mid+1, value);
        
        return binary_interval_search(mid, b, value);
    }
    
    template<typename T>
    [[nodiscard]] constexpr size_t distance(const T* a, const T* b) 
    {
        return (a > b) ?
            static_cast<size_t>(a - b) :
            static_cast<size_t>(b - a);
    }
}

#endif