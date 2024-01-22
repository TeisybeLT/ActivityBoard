#include "intro.hpp"
#include "buzzer.hpp"
#include "constants.hpp"
#include "debug.hpp"
#include "display_segments.hpp"

#include "util/delay.h"

void screens::play_intro()
{
    using namespace buzzer;
    static constexpr note notes[] =
    {
        note::C,
        note::B,
        note::C,
        note::G,
        note::F
    };
    static constexpr auto note_time = 200;
    
    DEBUG_PRINTF("Playing intro\r\n");
    
    // Print "Hi" in Lithuanian
    display::seg_13::map::write_text("LABAS", 2);
    display::draw();
    
    for (uint8_t idx = 0; idx < sizeof(notes); ++idx)
    {
        set_note(notes[idx]);
        _delay_ms(note_time);
    }
    
    set_note(note::rest);
    
    for (uint8_t light = 0;
        light < constants::default_backlight_brightness;
        ++light)
    {
        display::set_backlight_brightness(light);
        _delay_ms(3);
    }
}