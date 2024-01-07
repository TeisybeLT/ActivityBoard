#ifndef DEMO_SCREEN_HPP
#define DEMO_SCREEN_HPP

#include "i_screen.hpp"
#include "display_segments.hpp"

namespace screens
{
    struct demo_screen : public i_screen
    {
        demo_screen();

        void set_static_elements() override;
        void tick(input::bank_a_data input_a, input::bank_b_data input_b, int8_t rot_val) override;
        
    private:
        void handle_a_long_press(input::sw_bank_a button);
        void handle_a_short_press(input::sw_bank_a button);
        void handle_b_long_press(input::sw_bank_b button);
        void handle_b_short_press(input::sw_bank_b button);
        void handle_rot(int8_t rot_value);
        void set_next_setgment_position(bool forwards);
        void set_segment_at_position(uint8_t index);
        
        void handle_animation();
        void handle_backlight();

        uint8_t symbol_array[display::seg_13::digit_count]; 
        uint8_t current_digit_position = 0;
        uint8_t blink_counter = 0;
        int8_t spinner = 0;

        uint8_t animation_line_idx = 0;
        uint8_t animation_line_counter = 0;
        bool animation_line_dir = false;
        bool animation_line_active = true;
        
        uint8_t mode_index = 0;
        
        bool is_pulsing = false;
        bool backlight_falling = false;

        bool on_blink = false;
        bool redraw_needed = false;
    };
}

#endif