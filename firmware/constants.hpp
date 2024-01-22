#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include "input.hpp"
#include <avr/io.h>

namespace constants
{
    constexpr auto system_clock = uint32_t{F_CPU};
    constexpr auto default_backlight_brightness = uint8_t{0xFF};

    namespace pins
    {
        // Faceplate screen pins
        constexpr auto CLK = PORTB5;
        constexpr auto DATA = PORTB3;
        constexpr auto CE = PORTB2;
        constexpr auto nINH = PORTB0;
        constexpr auto backlight = PORTD6;
        
        // Buzzer
        constexpr auto buzz = PORTB1;
        
        // Input button pins
        constexpr auto KY1 = input::ADC0;
        constexpr auto KY2 = input::ADC1;
        constexpr auto VOLA = input::ADC2;
        
        // Debug UART
        constexpr auto RXD = PORTD0;
        constexpr auto TXD = PORTD1;
    }
}

#endif