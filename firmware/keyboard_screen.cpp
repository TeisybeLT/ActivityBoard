#include "keyboard_screen.hpp"

#include "debug.hpp"

#include <avr/sfr_defs.h>
#include "string.h"

using namespace screens;

keyboard_screen::keyboard_screen()
{
    input::set_a_mask(0xFF);
    input::set_b_mask(0xFFFF);
    
    using namespace display::seg_13;
    ::memset(symbol_array, symbol_count - 1, sizeof(symbol_array));
    redraw_needed = true;
}

keyboard_screen::~keyboard_screen()
{
    input::set_a_mask(0);
    input::set_b_mask(0);
}

void keyboard_screen::tick(input::bank_a_data, input::bank_b_data, int8_t)
{
    const auto raw_a = input::get_raw_a();
    const auto raw_b = input::get_raw_b();
    
    if ((raw_b != last_b_press) || raw_a != last_a_press)
    {
        switch(raw_b)
        {
            using enum input::sw_bank_b;

            case sw9:
                set_note(buzzer::note::C); 
                break;
            
            case sw10:
                set_note(buzzer::note::D);
                break;

            case sw11:
                set_note(buzzer::note::E); 
                break;

            case sw12:
                set_note(buzzer::note::F); 
                break;

            case sw13:
                set_note(buzzer::note::G); 
                break;

            case sw14:
                set_note(buzzer::note::A);
                break;
            
            default:
                if (input::get_raw_a() == input::sw_bank_a::sw15)
                {
                    set_note(buzzer::note::B);
                    break;
                }

                buzzer::set_note(buzzer::note::rest);
        }
    }

    last_a_press = raw_a;
    last_b_press = raw_b;
    
    if (redraw_needed)
    {
        display::draw();
        redraw_needed = false;
    }
}

void keyboard_screen::set_note(buzzer::note note)
{
    using namespace display::seg_13;
    static constexpr uint8_t converter[static_cast<size_t>(buzzer::note::rest)] = 
    {
        map::char_to_idx('C'),
        map::char_to_idx('D'),
        map::char_to_idx('E'),
        map::char_to_idx('F'),
        map::char_to_idx('G'),
        map::char_to_idx('A'),
        map::char_to_idx('B')
        
    };

    buzzer::set_note(note); 

    ::memmove(&symbol_array[1], symbol_array, sizeof(symbol_array) - 1);
    symbol_array[0] = converter[static_cast<uint8_t>(note)];
    for (uint8_t idx = 0; idx < digit_count; ++idx)
        map::write_segments(symbols[symbol_array[idx]], idx);
    
    redraw_needed = true;
}