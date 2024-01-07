#ifndef I_SCREEN_HPP
#define I_SCREEN_HPP

#include "input.hpp"

namespace screens
{
    struct i_screen
    {
        virtual void set_static_elements() = 0;
        virtual void tick(input::bank_a_data input_a, input::bank_b_data input_b, int8_t rot_val) = 0;
    };
}

#endif