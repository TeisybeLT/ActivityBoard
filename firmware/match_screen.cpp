#include "match_screen.hpp"
#include "buzzer.hpp"
#include "display_segments.hpp"
#include "globals.hpp"

#include <avr/sfr_defs.h>
#include <stdlib.h>

#include "debug.hpp"

using namespace screens;

namespace
{
    constexpr auto max_number = int8_t{99};
    
    void draw_number(int8_t number, int8_t position)
    {
        // Position is of the first digit!
        using namespace display;
        static constexpr auto number_offset = uint8_t{5};

        do
        {
            const auto digit = number % 10;
            number /= 10;
            
            seg_13::map::write_segments(seg_13::symbols[number_offset + digit], position);
            --position;
        } while(number);
    }
}

match_screen::match_screen()
{
    ::srand(globals::tick_counter);

    input::set_a_mask(0xFF);
    
    static constexpr auto rot_mask = ~uint16_t
    { _BV(static_cast<uint8_t>(input::sw_bank_b::rot)) };
    input::set_b_mask(rot_mask);
}

match_screen::~match_screen()
{
    input::set_a_mask(0);
    input::set_b_mask(0);
}

void match_screen::tick(input::bank_a_data input_a, input::bank_b_data input_b, int8_t rot_val)
{
    switch(current_state)
    {
        case game_state::start:
            start();
            break;
            
        case game_state::run:
            run(input_a, input_b, rot_val);
            break;
            
        case game_state::win:
            win();
            break;
            
        case game_state::lose:
            lose();
            break;
            
        case game_state::points:
            display_points();
            break;
    }
    
    if (redraw_needed)
        display::draw();
}

void match_screen::start()
{
    using namespace display;

    target_value = static_cast<int8_t>(::rand() % max_number);
    draw_number(target_value, 7);
    current_value = 0;
    seg_13::map::write_segments(seg_13::symbols[seg_13::symbol_count - 1], 0);
    draw_number(current_value, 1);
    current_state = match_screen::game_state::run;
    
    seg_13::map::write_segments(seg_13::symbols[3], 3);
    seg_13::map::write_segments(seg_13::symbols[3], 4);
    
    redraw_needed = true;
}

void match_screen::run(input::bank_a_data, input::bank_b_data input_b, int8_t rot_val)
{
    using namespace display;
    if (rot_val)
    {
        current_value += rot_val;
        if (current_value > max_number)
        {
            seg_13::map::write_segments(seg_13::symbols[seg_13::symbol_count - 1], 0);
            current_value -= max_number + 1;
        }
        else if (current_value < 0)
            current_value += max_number + 1;
        
        draw_number(current_value, 1);
        redraw_needed = true;
    }
    
    if (input_b.button == input::sw_bank_b::rot)
    {
        if (current_value == target_value)
        {
            streak = (streak < 255) ? streak + 1 : 0;                
            current_state = game_state::win;
        }
        else
        {
            streak = 0;
            current_state = game_state::lose;
        }
        
        seg_13::map::clear_segments();
        redraw_needed = true;
    }
}

void match_screen::win()
{
    if (process_counter == 30)
    {
        switch(beep_step)
        {
            case 0:
                buzzer::set_note(buzzer::note::C);
                display::seg_13::map::write_text("Taip", 2);
                redraw_needed = true;
                break;

            case 1:
                buzzer::set_note(buzzer::note::D);
                break;

            case 2:
                buzzer::set_note(buzzer::note::E);
                break;
            
            default:
                display::seg_13::map::clear_segments();
                buzzer::set_note(buzzer::note::rest);
                current_state = game_state::points;
                beep_step = 0;
                process_counter = 0;
                return;
        }
        
        process_counter = 0;
        ++beep_step;
    }

    ++process_counter;
}

void match_screen::lose()
{
    if (process_counter == 30)
    {
        switch(beep_step)
        {
            case 0:
                buzzer::set_note(buzzer::note::E);
                display::seg_13::map::write_text("Ne", 3);
                redraw_needed = true;
                break;

            case 1:
                buzzer::set_note(buzzer::note::D);
                break;

            case 2:
                buzzer::set_note(buzzer::note::C);
                break;
            
            default:
                buzzer::set_note(buzzer::note::rest);
                display::seg_13::map::clear_segments();
                current_state = game_state::points;
                beep_step = 0;
                process_counter = 0;
                return;
        }
        
        process_counter = 0;
        ++beep_step;
    }

    ++process_counter;
}

void match_screen::display_points()
{
    if (process_counter == 0)
    {
        draw_number(streak, 7);    
        redraw_needed = true;
    }
    
    if (process_counter >= 128)
    {
        display::seg_13::map::clear_segments();
        current_state = game_state::start;
        process_counter = 0;
    }
    else
        ++process_counter;
}