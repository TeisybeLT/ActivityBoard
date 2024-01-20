#include "screen_manager.hpp"

#include "constants.hpp"
#include "demo_screen.hpp"
#include "match_screen.hpp"
#include "keyboard_screen.hpp"
#include "heap.hpp"

#include "debug.hpp"

using namespace screens;

namespace
{
    constexpr auto max_screen_count = uint8_t{3};
    constexpr auto default_screen = uint8_t{0};

    alignas(screens::demo_screen) uint8_t current_screen_data[sizeof(screens::demo_screen)];
    uint8_t index = 0;
    
    i_screen* current_screen_ptr = nullptr;
    
    void set_screen(uint8_t new_index)
    {
        DEBUG_PRINTF("Setting screen to: %u\r\n", new_index);
        if (current_screen_ptr)
            current_screen_ptr->~i_screen();
        display::clear_all();
    
        switch(new_index)
        {
            case 0:
                current_screen_ptr = new (current_screen_data) demo_screen;
                break;

            case 1:
                current_screen_ptr = new (current_screen_data) match_screen;
                break;

            case 2:
                current_screen_ptr = new (current_screen_data) keyboard_screen;
                break;
        }

        display::set_backlight_brightness(constants::default_backlight_brightness);
        display::seg_7::map::write_segments(display::seg_7::symbols[new_index + 1]);
        index = new_index;
    }
}

screens::i_screen* screen_manager::init()
{
    set_screen(default_screen);
    return current_screen_ptr;
}

screens::i_screen* screen_manager::up()
{
    auto new_index = uint8_t{++index};
    new_index = new_index < max_screen_count ? new_index : 0;
    set_screen(new_index);
    return current_screen_ptr;
}

screens::i_screen* screen_manager::down()
{
    auto new_index = uint8_t{index};
    new_index = (new_index == 0) ? max_screen_count - 1 : new_index - 1;
    set_screen(new_index);        
    return current_screen_ptr;
}