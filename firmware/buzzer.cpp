#include "buzzer.hpp"
#include "constants.hpp"
#include "debug.hpp"

#include <avr/io.h>
#include <stddef.h>
#include <util/atomic.h>

namespace
{
    constexpr auto tone_gen_divider = uint8_t{1};
    constexpr auto C_note_freq = 2048.0;
    constexpr auto note_interval = 1.1224620602616102; // 8 notes/octave
    constexpr auto pule_length_ratio = 0.25;
    constexpr auto beep_for_timer_prescaler = uint8_t{0b111 << CS20};
    
    consteval uint16_t freq_to_tmr(double freq)
    {
        return uint16_t(constants::system_clock / (2 * tone_gen_divider * freq));
    }
    
    template<buzzer::note Note>
    consteval uint16_t compute_note_timer_max()
    {
        using namespace buzzer;

        constexpr auto note_id = static_cast<uint8_t>(Note);
        static_assert(note_id < static_cast<uint8_t>(note::rest));
        
        // Poor-man's pow implementation
        if (Note == note::C)
            return freq_to_tmr(C_note_freq);
        
        auto out = C_note_freq;
        for (uint8_t idx = 0; idx < note_id; ++idx)
            out *= note_interval;
        
        return freq_to_tmr(out);
    }
    
    // Yes, I know I should have implemented std::array, this would have cleaned this
    // by quite a bit. I am too lazy at this point in time
    constexpr uint16_t tone_timer_table[static_cast<size_t>(buzzer::note::rest)] =
    {
        compute_note_timer_max<buzzer::note::C>(),
        compute_note_timer_max<buzzer::note::D>(),
        compute_note_timer_max<buzzer::note::E>(),
        compute_note_timer_max<buzzer::note::F>(),
        compute_note_timer_max<buzzer::note::G>(),
        compute_note_timer_max<buzzer::note::A>(),
        compute_note_timer_max<buzzer::note::B>(),
    };

    template<buzzer::note Note>
    consteval uint16_t compute_tone_length()
    {
        using namespace buzzer;

        constexpr auto note_id = static_cast<uint8_t>(Note);
        static_assert(note_id < static_cast<uint8_t>(note::rest));
        
        constexpr auto tone_max = double(tone_timer_table[note_id]);
        constexpr auto offset = tone_max * (pule_length_ratio);
        
        // TODO: Implement correction for non-linear speaker curve
        
        return static_cast<uint16_t>(tone_max - offset);
    }
    
    constexpr uint16_t tone_length_table[static_cast<size_t>(buzzer::note::rest)] =
    {
        compute_tone_length<buzzer::note::C>(),
        compute_tone_length<buzzer::note::D>(),
        compute_tone_length<buzzer::note::E>(),
        compute_tone_length<buzzer::note::F>(),
        compute_tone_length<buzzer::note::G>(),
        compute_tone_length<buzzer::note::A>(),
        compute_tone_length<buzzer::note::B>(),
    };

    volatile auto is_playing = false;

    void set_note_regs(buzzer::note note)
    {
        const auto id = static_cast<uint8_t>(note);
        ICR1 = tone_timer_table[id];
        OCR1A = tone_length_table[id];
    }

    void start_buzz()
    {
        static_assert(tone_gen_divider < 8);
        TCCR1B = TCCR1B | (tone_gen_divider << CS10);
    }
    
    void stop_buzz()
    {
        static constexpr auto clk_disable_mask = 
            ~uint8_t{(_BV(CS12) | _BV(CS11) | _BV(CS10))};

        TCCR1B = TCCR1B & clk_disable_mask;
    }
    
    ISR(TIMER2_COMPA_vect)
    {
        TCCR2B = TCCR2B & ~beep_for_timer_prescaler;
        stop_buzz(); 
        is_playing = false;
    }
}

void buzzer::init()
{
    // Timer1 is used to generate the actual tone that will be played
    // out of the buzzer
    
    // Set PB1 as output (the timer pin)
    DDRB = DDRB | _BV(DDB1);

    // OC1A is the output for the timer (to buzzer)
    TCCR1A = TCCR1A | (0b11 << COM1A0);
    
    // Select phase and frequency correct mode
    TCCR1B = TCCR1B | (0b10 << WGM12);
    
    // Timer2 is used for beep-for functionality
    // Select CTC mode for Timer2
    TCCR2A = TCCR2A | (0b10 << WGM20);

    // Enable the interrupt for Timer2 overflow
    TIMSK2 = TIMSK2 | _BV(OCIE2A);
    
    DEBUG_PRINTF("Buzzer init OK\r\n");
}

void buzzer::set_note(buzzer::note note)
{
    if (note == buzzer::note::rest)
    {
        stop_buzz();
        ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
        {
            is_playing = false;
        }
    }
    
    set_note_regs(note);
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        // Just to make sure, stop the timer.
        // This is done to solve a race condition when beep finishes
        // but a somebody else has requested for the buzzer to output
        // a continuous note
        TCCR2B = TCCR2B & ~beep_for_timer_prescaler;

        if (!is_playing)
        {
            start_buzz();
            is_playing = true;            
        }
    }
}

void buzzer::set_note_for(buzzer::note note, uint8_t timer_ticks)
{
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
    {
        if (is_playing)
            return;
        
        is_playing = true;
    }
    
    // Set up Timer2 limits
    TCNT2 = 0;
    OCR2A = timer_ticks;
    
    // Enable Timer2 by setting the prescaler to 1024
    TCCR2B = TCCR2B | beep_for_timer_prescaler;
    
    set_note_regs(note);
    start_buzz();
}