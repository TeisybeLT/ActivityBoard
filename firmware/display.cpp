#include "display.hpp"
#include "debug.hpp"
#include "constants.hpp"

#include <avr/interrupt.h>
#include <util/atomic.h>

namespace
{
    // Address is taken from Sanyo LC75823E Datasheet
    constexpr auto ccb_address = uint8_t{0x41};
    
    constexpr auto data_byte_count = size_t{20};
    uint8_t data_buffer[data_byte_count] = {0};
    volatile uint8_t data_in_transfer[data_byte_count] = {0};
    
    volatile auto draw_in_progress = false;
    
    ISR(SPI_STC_vect)
    {
        static volatile auto write_index = uint8_t{0};

        if (write_index == data_byte_count)
        {
            write_index = 0;
            PORTB = PORTB & ~_BV(constants::pins::CE);
            draw_in_progress = false;
            return;
        }
        
        if (write_index == 0)
        {
            PORTB = PORTB | _BV(constants::pins::CE);
        }
        
        SPDR = data_in_transfer[write_index];
        write_index = write_index + 1;
    }
}

void display::set_segment(display::seg_address address)
{
    data_buffer[address.byte] = data_buffer[address.byte] | _BV(address.bit);
}

void display::clear_segment(display::seg_address address)
{
    data_buffer[address.byte] = data_buffer[address.byte] & ~static_cast<uint8_t>((1 << address.bit));
}

void display::toggle_segment(display::seg_address address)
{
    data_buffer[address.byte] = data_buffer[address.byte] ^ _BV(address.bit);
}

void display::enable_screen()
{
    PORTB = PORTB | _BV(constants::pins::nINH);
}

void display::disable_screen()
{
    PORTB = PORTB & ~_BV(constants::pins::nINH);
}

void display::set_backlight_brightness(uint8_t value)
{
    OCR0A = value;
}

uint8_t display::get_backlight_brightness()
{
    return OCR0A;
}

void display::init()
{
    // Configure mode of nINH and CE pin
    static constexpr auto port_b_bit_mask = _BV(DDB5) | _BV(DDB3) | _BV(DDB2) | _BV(DDB1);
    DDRB = DDRB | port_b_bit_mask;

    // Configure serial transmitter
    static constexpr auto spi_bit_mask = _BV(SPIE) | _BV(SPE) | _BV(DORD) | _BV(MSTR);
    SPCR = SPCR | spi_bit_mask;

    // Initial condition is screen is inhibited and the display controller chip is not selected
    display::disable_screen();
    PORTB = PORTB & ~_BV(constants::pins::CE);
    
    // Configure PWM output for the LCD backlight. Use Fast PWM mode
    DDRD = DDRD | _BV(constants::pins::backlight);
    TCCR0A = TCCR0A | (0b10 << COM0A0) | (0b11 << WGM00);
    TCCR0B = TCCR0B | (0b010 << CS00);
    display::set_backlight_brightness(0); // Default value - backlight off(ish)
    
    DEBUG_PRINTF("Display controller init ok\r\n");
}

void display::draw()
{
    if (draw_in_progress)
        return;
    
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        for (size_t idx = 0; idx < data_byte_count; ++idx)
            data_in_transfer[idx] = data_buffer[idx];
    }

    SPDR = ccb_address;
    draw_in_progress = true;
}
