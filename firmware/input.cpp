#include "input.hpp"
#include "algorithms.hpp"
#include "buzzer.hpp"
#include "constants.hpp"
#include "debug.hpp"
#include "utils.hpp"

#include <util/atomic.h>
#include <util/delay.h>

namespace
{
    constexpr auto short_press_beep_ticks = uint8_t{64};
    constexpr auto long_press_beep_ticks = uint8_t{255};
    constexpr auto button_note = buzzer::note::C;

    void set_adc_channel(input::adc_mux target)
    {
        static constexpr auto mux_mask = uint8_t{0b11110000};
        ADMUX = (ADMUX & mux_mask) | static_cast<uint8_t>(target);
    }
    
    void disable_digital_input(input::adc_mux target)
    {
        DIDR0 = DIDR0 | (_BV(static_cast<uint8_t>(target)) & 0b11000000);
    }
    
    // These values are measured from a particular ADC channel using an external
    // pull-up resistor. Be careful when modifying these look up tables
    constexpr int16_t sw_bank_a_lookup[static_cast<size_t>(input::sw_bank_a::MAX)] = 
    {35, 79, 135, 207, 291, 391, 496, 743, 1023};
    constexpr const int16_t* sw_bank_a_lookup_end = sw_bank_a_lookup + utils::array_size(sw_bank_a_lookup);

    constexpr int16_t sw_bank_b_lookup[static_cast<size_t>(input::sw_bank_b::MAX)] = 
    {0, 33, 79, 135, 207, 287, 387, 496, 600, 696, 920, 1023};
    constexpr const int16_t* sw_bank_b_lookup_end = sw_bank_b_lookup + utils::array_size(sw_bank_b_lookup);
    
    constexpr int16_t rot_low[2] = {149, 213};
    constexpr int16_t rot_mid[2] = {533, 597};
    constexpr int16_t rot_high[2] = {962, 1023};
    
    enum class rot_range : uint8_t
    {
        undefined,
        low,
        mid,
        high,
    };

    auto bank_a_buffer = input::bank_a_data
    {
        .button = input::sw_bank_a::none,
        .is_long = false
    };
    auto bank_b_buffer = input::bank_b_data
    {
        .button = input::sw_bank_b::none,
        .is_long = false
    };

    auto rotary_buffer = int8_t{0};

    [[nodiscard]] int16_t sample_channel(input::adc_mux channel)
    {
        static constexpr auto ch_change_delay = 500; // us

        // Set channel    
        set_adc_channel(channel);
        _delay_us(ch_change_delay);

        // Start sample and wait for it to finish
        ADCSRA = ADCSRA | _BV(ADSC);
        loop_until_bit_is_clear(ADCSRA, ADSC);
        
        return static_cast<int16_t>(ADC);
    }

    [[nodiscard]] size_t sample_and_get_array_index(const int16_t* begin, const int16_t* end,
        input::adc_mux channel)
    {
        const auto value = sample_channel(channel);
        const auto* iter = algorithms::binary_interval_search(begin, end, value);
        return algorithms::distance(iter, begin);
    }
    
    [[nodiscard]] rot_range sample_to_rotary_range(int16_t value)
    {
        if (algorithms::in_closed_range(rot_low[0], rot_low[1], value))
            return rot_range::low;
        
        if (algorithms::in_closed_range(rot_mid[0], rot_mid[1], value))
            return rot_range::mid;
        
        if (algorithms::in_closed_range(rot_high[0], rot_high[1], value))
            return rot_range::high;
        
        return rot_range::undefined;
    }
    
    template<typename BankEnum, typename Buffer>
    void tick_generic(size_t key_index, Buffer& key_buffer)
    {
        static auto hold_count = uint8_t{0};
        static auto long_posted = false;
        static auto previous_press = BankEnum::none;
        
        const auto current_press = static_cast<BankEnum>(key_index);

        if((previous_press == BankEnum::none) && (current_press != BankEnum::none))
        {
            // Key down event
            hold_count = 0;
            long_posted = false;
        }
        else if((previous_press != BankEnum::none) && (current_press == BankEnum::none))
        {
            // Key up event
            if (hold_count < input::ticks_for_long_press)
            {
                key_buffer.is_long = false;
                key_buffer.button = previous_press;
                buzzer::set_note_for(button_note, short_press_beep_ticks);
            }
        }
        else if ((previous_press != BankEnum::none) && (previous_press == current_press))
        {
            // Key hold
            if (hold_count != 0xFF)
                ++hold_count;
            
            if ((hold_count >= input::ticks_for_long_press) && !long_posted)
            {
                long_posted = true;
                key_buffer.is_long = true;
                key_buffer.button = current_press;
                buzzer::set_note_for(button_note, long_press_beep_ticks);
            }
        }
            
        previous_press = current_press;
    }

    void tick_rotary(int16_t sample)
    {
        static auto previous_range = rot_range::undefined;
    
        const auto current_range = sample_to_rotary_range(sample);
        if (current_range == previous_range)
            return;     
        
        if (((previous_range == rot_range::mid) && (current_range == rot_range::high)) ||
            ((previous_range == rot_range::high) && (current_range == rot_range::low)))
        {
            // Rotation clock-wise
            // To detect a CW rotation, the transition patterns are either:
            // * Low->mid->high (can be simplified to mid->high as is the case here)
            // * High->low
            rotary_buffer = (rotary_buffer != 127) ? rotary_buffer + 1 : rotary_buffer;
            buzzer::set_note_for(button_note, short_press_beep_ticks);
        }
        else if (((previous_range == rot_range::mid) && (current_range == rot_range::low)) ||
            ((previous_range == rot_range::low) && (current_range == rot_range::high)))
        {
            // Rotation counter clock-wise
            // Rotation clock-wise
            // To detect a CCW rotation, the transition patterns are either:
            // * High->mid->low (can be simplified to high->low as is the case here)
            // * Low->high
            rotary_buffer = (rotary_buffer != -128) ? rotary_buffer - 1 : rotary_buffer;
            buzzer::set_note_for(button_note, short_press_beep_ticks);
        } 

        previous_range = current_range;
    }
}

void input::init()
{
    using namespace constants;

    // Power digital input buffers
    disable_digital_input(pins::KY1);
    disable_digital_input(pins::KY2);
    disable_digital_input(pins::VOLA);

    // Set Vref to Vcc. Since the buttons are a resistor ladder,the precision of Vref isn't crucial
    // that's why Vcc will suffice.
    // Clock divider is also set to 64, which gives a sampling freq of 125kHz
    ADMUX = ADMUX | (0b01 << REFS0) | (0b110 << ADPS0);
    
    // Enable ADC
    ADCSRA = ADCSRA | _BV(ADEN);
    
    // Do sampling once after enable and discard results
    // This is done to allow proper initialization of sampling logic
    (void)sample_channel(pins::KY1);
    (void)sample_channel(pins::KY2);
    (void)sample_channel(pins::VOLA);
    
    DEBUG_PRINTF("Input controller init ok\r\n");
}

// Tick samples multiple channels and due to high impedance of the
// input, sample-and-hold capacitor does not have enough time to charge
// during interrupts. Therefore, this function is not interrupt driven and
// utilizes busy-wait
void input::tick()
{
    using namespace constants;
    
    // Inputs are only sampled if there is no button press data in buffer already
    if (bank_a_buffer.button == sw_bank_a::none)
    {
        const auto idx = sample_and_get_array_index(sw_bank_a_lookup, sw_bank_a_lookup_end, pins::KY1);
        tick_generic<input::sw_bank_a>(idx, bank_a_buffer);
    }

    if (bank_b_buffer.button == sw_bank_b::none)
    {
        const auto idx = sample_and_get_array_index(sw_bank_b_lookup, sw_bank_b_lookup_end, pins::KY2);
        tick_generic<input::sw_bank_b>(idx, bank_b_buffer);
    }
    
    const auto rotary_adc = sample_channel(pins::VOLA);
    tick_rotary(rotary_adc);
}

input::bank_a_data input::pop_bank_a()
{
    const auto out = bank_a_buffer;

    bank_a_buffer = bank_a_data
    {
        .button = input::sw_bank_a::none,
        .is_long = false
    };

    return out;
}

input::bank_b_data input::pop_bank_b()
{
    const auto out = bank_b_buffer;

    bank_b_buffer = bank_b_data
    {
        .button = input::sw_bank_b::none,
        .is_long = false
    };
    
    return out;
}

int8_t input::pop_rotary()
{
    const auto out = rotary_buffer;
    rotary_buffer = 0;
    return out;
}