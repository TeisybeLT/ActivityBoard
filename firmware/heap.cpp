#include "debug.hpp"

#include <stdlib.h>
#include <stdint.h>

// Please note that these heap functions are not intended to
// be used. Therefore, they have debug statements to warn
// the caller of these functions. The goal of these heap
// functions is to make avr-gcc shut up when used with
// virtual destructors.

void* operator new(size_t size)
{
    if (size == 0)
        ++size;

    DEBUG_PRINTF("::new called!\r\n");
    return ::malloc(size);
}

void* operator new[](size_t size)
{
    if (size == 0)
        ++size;

    DEBUG_PRINTF("::new[] called!\r\n");
    return ::malloc(size);
}

void operator delete(void* ptr)
{
    DEBUG_PRINTF("::delete called!\r\n");
    ::free(ptr);
}

void operator delete(void* ptr, size_t)
{
    DEBUG_PRINTF("Sized ::delete called!\r\n");
    ::free(ptr);
}


void operator delete[](void* ptr)
{
    DEBUG_PRINTF("::delete[] called!\r\n");
    ::free(ptr);
}

void operator delete[](void* ptr, size_t)
{
    DEBUG_PRINTF("Sized ::delete[] called!\r\n");
    ::free(ptr);
}