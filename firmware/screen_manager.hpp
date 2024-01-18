#ifndef SCREEN_MANAGER
#define SCREEN_MANAGER

#include "i_screen.hpp"

namespace screens
{
    struct screen_manager
    {
        static screens::i_screen* init();

        static screens::i_screen* up();
        static screens::i_screen* down();
    };
}

#endif