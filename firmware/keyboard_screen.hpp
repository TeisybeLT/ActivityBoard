#ifndef KEYBOARD_SCREEN
#define KEYBOARD_SCREEN

#include "buzzer.hpp"
#include "display_segments.hpp"
#include "i_screen.hpp"
#include "input.hpp"

namespace screens
{
    struct keyboard_screen : public i_screen
    {
        keyboard_screen();
        ~keyboard_screen();

        void tick(input::bank_a_data input_a, input::bank_b_data input_b, int8_t rot_val) override;

    private:
        void set_note(buzzer::note note);

        uint8_t symbol_array[display::seg_13::digit_count]; 
        input::sw_bank_b last_b_press = input::sw_bank_b::none;
        input::sw_bank_a last_a_press = input::sw_bank_a::none;
        bool redraw_needed;
    };
}

#endif