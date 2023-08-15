#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <stdint.h>
#include <avr/io.h>

namespace constants
{
    constexpr auto system_clock = uint32_t{F_CPU};
    
    namespace pins
    {
        // Faceplate screen pins
        constexpr auto CE = PORTB2;
        constexpr auto nINH = PORTB1;
        constexpr auto backlight = PORTD6;
    }
}

#endif