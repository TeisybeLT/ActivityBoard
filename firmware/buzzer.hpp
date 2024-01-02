#ifndef BUZZER_HPP
#define BUZZER_HPP

#include <stdint.h>

namespace buzzer
{
    enum class note : uint8_t
    {
        C, D, E, F, G, A, B,
        rest // Also doubles as MAX value
    };
    void init();
    void set_note(note note);
    void set_note_for(note note, uint8_t timer_ticks);
}

#endif