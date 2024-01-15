#ifndef MATCH_SCREEN_HPP
#define MATCH_SCREEN_HPP

#include "i_screen.hpp"

namespace screens
{
    struct match_screen : public i_screen
    {
        enum class game_state : uint8_t
        {
            start,
            run,
            win,
            lose,
            points
        };

        match_screen();
        ~match_screen();
    
        void tick(input::bank_a_data input_a, input::bank_b_data input_b, int8_t rot_val) override;
    private:
        void start();
        void run(input::bank_a_data input_a, input::bank_b_data input_b, int8_t rot_val);
        void win();
        void lose();
        void display_points();

        game_state current_state = game_state::start;
        int8_t current_value = 0;
        int8_t target_value = 0;
        uint8_t process_counter = 0;
        uint8_t beep_step = 0;
        uint8_t streak = 0;
        bool redraw_needed = false;
    };
}

#endif