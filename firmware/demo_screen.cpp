#include "demo_screen.hpp"
#include "debug.hpp"
#include <string.h>

using namespace screens;
using namespace display;

namespace
{
    constexpr auto empty_symbol = display::seg_13::symbol_count - 1;
    constexpr auto underscore_symbol = display::seg_13::symbol_count - 2;
    constexpr auto user_symbol_end = underscore_symbol;
    
    constexpr display::cd_spinner::IndexType spinner_values[] =
    {
        0b001,
        0b010,
        0b100,
        0b000
    };
    constexpr uint8_t anim_bits[] =
    {
        0b0000000,
        0b1000000,
        0b1100000,
        0b1110000,
        0b1111000,
        0b1111100,
        0b1111110,
        0b1111111,
    };
    constexpr seg_address modes[]
    {
        icons::rock,
        icons::pop,
        icons::cls,
        icons::dsp
    };

    uint8_t number_to_seg13_index(uint8_t index)
    {
        return index + 5;
    }
}

demo_screen::demo_screen()
{
    (void)::memset(symbol_array, empty_symbol, sizeof(symbol_array));

    set_segment(cd_spinner::center_s_cdp);
    set_backlight_brightness(0xFF);
}

demo_screen::~demo_screen()
{
}

void demo_screen::tick(input::bank_a_data input_a, input::bank_b_data input_b, int8_t rot_val)
{
    // Dispatch buttons
    if (input_a.button != input::sw_bank_a::none)
    {
        if (input_a.is_long)
            handle_a_long_press(input_a.button);
        else
            handle_a_short_press(input_a.button);
    }
    
    if (input_b.button != input::sw_bank_b::none)
    {
        if (input_b.is_long)
            handle_b_long_press(input_b.button);
        else
            handle_b_short_press(input_b.button);
    }
    
    if (rot_val)
        handle_rot(rot_val);
    
    
    if (blink_counter > 0x80)
    {
        if (on_blink)
            seg_13::map::write_segments(seg_13::symbols[symbol_array[current_digit_position]], current_digit_position);
        else 
            seg_13::map::write_segments(seg_13::symbols[underscore_symbol], current_digit_position);
        
        on_blink = !on_blink;
        blink_counter -= 0x80;
        redraw_needed = true;
    }
    
    handle_animation();
    handle_backlight();
    
    if (redraw_needed)
    {
        draw();
        redraw_needed = false;
    }
    
    ++blink_counter;
    
    if (animation_line_active)
        ++animation_line_counter;
}

void demo_screen::handle_a_long_press(input::sw_bank_a button)
{
    switch(button)
    {
        using enum input::sw_bank_a;

        case sw18:
            toggle_segment(icons::camera);
            break;
            
        case sw4:
            is_pulsing = !is_pulsing;
            break;

        case sw17:
            toggle_segment(icons::af);
            break;

        case sw16:
            toggle_segment(icons::ta);
            break;

        case sw15:
            toggle_segment(icons::pty);
            break;

        case sw7:
        {
            if (mode_index > 0)
            {
                clear_segment(modes[mode_index - 1]);
                mode_index = 0;
                redraw_needed = true;
            }
            break;
        }
        
        case sw2:
            toggle_segment(icons::stereo);
            break;

        case sw3:
            toggle_segment(icons::casette);
            break;

        default:
            DEBUG_PRINTF("Unknown Bank A long press: %u\r\n", static_cast<uint8_t>(button));
    }
}

void demo_screen::handle_a_short_press(input::sw_bank_a button)
{
    auto set_digit_helper = [&](uint8_t input)
    {
        set_segment_at_position(number_to_seg13_index(input));
        set_next_setgment_position(true);
    };

    switch(button)
    {
        using enum input::sw_bank_a;

        case sw18:
            set_digit_helper(0);
            break;
        

        case sw17:
            set_digit_helper(9);
            break;
        
        case sw16:
            set_digit_helper(8);
            break;

        case sw15:
            set_digit_helper(7);
            break;

        case sw7:
        {
            if (mode_index < utils::array_size(modes))
            {
                set_segment(modes[mode_index]);
                if (mode_index)
                    clear_segment(modes[mode_index - 1]);
                ++mode_index;
            }
            else
            {
                clear_segment(modes[mode_index - 1]);
                mode_index = 0;
            }

            redraw_needed = true;
            break;
        }

        default:
            DEBUG_PRINTF("Unknown Bank A short press: %u\r\n", static_cast<uint8_t>(button));
    }
}

void demo_screen::handle_b_long_press(input::sw_bank_b button)
{
    switch(button)
    {
        using enum input::sw_bank_b;

        case sw9:
            toggle_segment(icons::mp3);
            break;

        case sw10:
            toggle_segment(icons::eon);
            break;

        case sw11:
            toggle_segment(icons::tp);
            break;

        case sw12:
            toggle_segment(icons::loc);
            break;
    
        case sw6:
            animation_line_active = !animation_line_active; 
            break;

        case rot:
            set_segment_at_position(empty_symbol);
            break;
            
        case sw8:
            toggle_segment(icons::swf);
            break;
            
        case sw1:
        case sw5:
            DEBUG_PRINTF("Long press on Up/Down\r\n");
            break;

        default:
            DEBUG_PRINTF("Unknown Bank B long press: %u\r\n", static_cast<uint8_t>(button));
    }
}

void demo_screen::handle_b_short_press(input::sw_bank_b button)
{
    auto set_digit_helper = [&](input::sw_bank_b input)
    {
        // The numbers in sw_bank_b are layed out in order and start
        // at 0. Therefore, we can convert them directly to numbers
        // by offsetting it by one (sw9 is button labeled 1)
        const auto index = uint8_t(static_cast<uint8_t>(input) + 1);
        set_segment_at_position(number_to_seg13_index(index));
        set_next_setgment_position(true);
    };

    switch(button)
    {
        using enum input::sw_bank_b;

        case sw9:
        case sw10:
        case sw11:
        case sw12:
        case sw13:
        case sw14:
            set_digit_helper(button);
            break;
        
        case sw6:
            toggle_segment(icons::loud);
            break;
        
        case sw5:
            set_next_setgment_position(false);
            break;
            
        case sw1:
        case rot:
            set_next_setgment_position(true);
            break;

        case sw8:
            toggle_segment(icons::reg);
            break;
            
        default:
            DEBUG_PRINTF("Unknown Bank B short press: %u\r\n", static_cast<uint8_t>(button));
    }
}


void demo_screen::handle_rot(int8_t rot_value)
{
    // Set the symbol
    auto current_symbol = static_cast<int8_t>(symbol_array[current_digit_position]) + rot_value;
    if (current_symbol >= static_cast<int8_t>(user_symbol_end))
        current_symbol -= static_cast<int8_t>(user_symbol_end);
    else if(current_symbol < 0)
        current_symbol += user_symbol_end;
    

    set_segment_at_position(static_cast<uint8_t>(current_symbol));
    
    // Handle the spinner
    cd_spinner::map::write_segments(spinner_values[3]);
    if (rot_value > 0)
        spinner = (spinner == 2) ? 0 : spinner + 1;
    else
        spinner = (spinner == 0) ? 2 : spinner - 1;

    cd_spinner::map::write_segments(spinner_values[spinner]);
}

void demo_screen::set_next_setgment_position(bool forwards)
{
    // Force-write current symbol to the current position
    seg_13::map::write_segments(seg_13::symbols[symbol_array[current_digit_position]],
        current_digit_position);

    if (forwards)
    {
        current_digit_position = (current_digit_position < seg_13::digit_count - 1) ?
            current_digit_position + 1 : 0;
    }
    else
    {
        current_digit_position = (current_digit_position != 0) ?
            current_digit_position - 1 : seg_13::digit_count - 1;
    }

    // Always show underline on new symbol
    seg_13::map::write_segments(seg_13::symbols[underscore_symbol], current_digit_position);
    redraw_needed = true;
    blink_counter = 0;
    on_blink = false;
}

void demo_screen::handle_animation()
{
    static constexpr auto anim_tick_count = 0x12;
    
    if (animation_line_counter >= anim_tick_count)
    {
        animation_line_idx = animation_line_dir ? animation_line_idx - 1 : animation_line_idx + 1;
        if (animation_line_idx == 0 || animation_line_idx == 7)
            animation_line_dir = !animation_line_dir;
        animation_line::map::write_segments(anim_bits[animation_line_idx]);
        animation_wheel::map::write_segments((~anim_bits[animation_line_idx]) & 0b1111111);

        animation_line_counter -= anim_tick_count;
        redraw_needed = true;
    }
}

void demo_screen::set_segment_at_position(uint8_t index)
{
    symbol_array[current_digit_position] = index;
    seg_13::map::write_segments(seg_13::symbols[index], current_digit_position);
    blink_counter = 0;
    on_blink = false;
    redraw_needed = true;
}

void demo_screen::handle_backlight()
{
    if (is_pulsing)
    {
        const auto cur_brightness = get_backlight_brightness();
        if ((cur_brightness == 0) || (cur_brightness == 0xFF))
            backlight_falling = !backlight_falling;
        
        const auto new_brightness = uint8_t(backlight_falling ?
            cur_brightness - 1 :
            cur_brightness + 1);
        set_backlight_brightness(new_brightness);
    }
}