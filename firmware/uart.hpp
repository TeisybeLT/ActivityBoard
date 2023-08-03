#ifndef UART_HPP
#define UART_HPP

#include "constants.hpp"

#include <avr/io.h>
#include <avr/sfr_defs.h>
#include <stdio.h>

// UART is implemented only in TX Mode (MCU sends data out)
// Format of this serial transmission is hard-coded to 8 data and 1 stop bit

namespace uart
{
    enum class baudrate : uint16_t
    {
        B2400 = 2400,
        B4800 = 4800,
        B9600 = 9600,
        B14400 = 14400,
        B19200 = 19200,
        B28800 = 28800,
        B38400 = 38400,
        B57600  = 57600
    };

    template <baudrate baud>
    void init()
    {
        static constexpr auto ubrr = uint16_t{constants::system_clock / (16 * static_cast<uint16_t>(baud)) - 1};

        // Set the baud rate
        UBRR0H = static_cast<uint8_t>(ubrr >> 8);
        UBRR0L = static_cast<uint8_t>(ubrr & 0xFF);
        
        // Enable TX mode
        UCSR0B = _BV(TXEN0);
        
        // Set the frame format to 8 data bits and 1 stop bit
        UCSR0C = (0b011 << UCSZ00);
    }
    
    int putc(char ch, ::FILE* stream);

    template<uart::baudrate baud>
    void init_as_stdout()
    {
        uart::init<baud>();

        static auto uart_stdout = ::FILE {
            .buf = nullptr,
            .unget = 0,
            .flags = _FDEV_SETUP_WRITE,
            .size = 0,
            .len = 0,
            .put = +[](char ch, ::FILE* stream){return uart::putc(ch, stream);},
            .get = nullptr,
            .udata = nullptr
        };
        
        stdout = &uart_stdout;       
    }
}

#endif