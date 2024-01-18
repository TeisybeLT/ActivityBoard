#include "buzzer.hpp"
#include "debug.hpp"
#include "display.hpp"
#include "globals.hpp"
#include "input.hpp"
#include "screen_manager.hpp"

#ifdef DEBUG_UART
    #include "uart.hpp"
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include "util/delay.h"

FUSES =
{
    // Calibrated internal RC oscillator, default start up time
    .low = FUSE_CKSEL0 & FUSE_CKSEL2 & FUSE_CKSEL3 & FUSE_SUT0,

    // Defaults
    .high = HFUSE_DEFAULT,
    .extended = EFUSE_DEFAULT 
};

int main()
{
#ifdef DEBUG_UART
    uart::init_as_stdout<uart::baudrate::B2400>();
#endif

    sei();
    buzzer::init();
    display::init();
    input::init();
    DEBUG_PRINTF("Init done\r\n");

    display::enable_screen();
    
    auto cur_screen = screens::screen_manager::init();
    
    while(true)
    {
        input::tick();

        const auto input_a = input::pop_bank_a();
        const auto input_b = input::pop_bank_b();        
        const auto rot_val = input::pop_rotary();        
        
        if (input_b.is_long)
        {
            if (input_b.button == input::sw_bank_b::sw1)
                cur_screen = screens::screen_manager::up(); 
            else if(input_b.button == input::sw_bank_b::sw5)
                cur_screen = screens::screen_manager::down(); 
        }

        cur_screen->tick(input_a, input_b, rot_val);
        ++globals::tick_counter;

        _delay_ms(5);
    }

    return 0;
}