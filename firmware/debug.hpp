#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <stdio.h>

#ifdef DEBUG_UART
    #define DEBUG_PRINTF(...) ::printf(__VA_ARGS__)
#else
    #define DEBUG_PRINTF(...)
#endif

#endif