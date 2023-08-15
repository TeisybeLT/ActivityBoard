#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <stdint.h>

// Display controller is utilizing transmit parts of SPI hardware (MISO is disregarded)
// This functionality can also be implemented using USART in SPI mode, but I wanted to
// have the option of having a debug UART, so SPI is used instead.

// Due to the nature of the display controller IC, SS control is also handled manually

namespace display
{
    struct seg_address
    {
        uint8_t byte : 5;
        uint8_t bit : 3;
    };
    static_assert(sizeof(seg_address) == sizeof(uint8_t));

    void set_segment(seg_address address);
    void clear_segment(seg_address address);
    void toggle_segment(seg_address address);

    void enable_screen();
    void disable_screen();

    void set_backlight_brightness(uint8_t value);
    uint8_t get_backlight_brightness();
    
    void init();
    
    // Do not call draw from an interrupt
    void draw();
}

#endif