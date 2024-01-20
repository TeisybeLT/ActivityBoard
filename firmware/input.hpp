#ifndef INPUT_HPP
#define INPUT_HPP

#include <stdint.h>

namespace input
{
    // The ordering of these two enums matter. It must be structured
    // based on measured ADC values from lowest to highest.
    // This is done in order to have 1-1 mapping of the underlying
    // look-up table. See input.cpp file for more info.
    // These enums must also contain "none" and "MAX" values
    enum class sw_bank_a : uint8_t
    {
        sw18 = 0,
        sw4,
        sw17,
        sw16,
        sw15,
        sw7,
        sw2,
        sw3,
    
        none,

        MAX
    };

    enum class sw_bank_b : uint8_t
    {
        sw9 = 0,
        sw10,
        sw11,
        sw12,
        sw13,
        sw14,
        sw6,
        sw1,
        sw5,
        rot,
        sw8,
    
        none,

        MAX
    };

    namespace detail
    {
        template<typename ButtonEnum> requires(sizeof(ButtonEnum) == sizeof(uint8_t))
        struct bank_data
        {
            ButtonEnum button : 7;
            bool is_long : 1;
        };
    }
    
    using bank_a_data = detail::bank_data<sw_bank_a>;
    using bank_b_data = detail::bank_data<sw_bank_b>;

    enum adc_mux : uint8_t
    {
        ADC0 = 0b0000,
        ADC1 = 0b0001,
        ADC2 = 0b0010,
        ADC3 = 0b0011,
        ADC4 = 0b0100,
        ADC5 = 0b0101,
        ADC6 = 0b0110,
        ADC7 = 0b0111,
        TEMP = 0b1000,
        Vbg = 0b1110,
        GND = 0b1111
    };
    
    constexpr auto ticks_for_long_press = uint8_t{128};

    // Input must be initialized after buzzer init has completed
    void init();
    void tick();
    
    [[nodiscard]] bank_a_data pop_bank_a();
    [[nodiscard]] bank_b_data pop_bank_b();
    [[nodiscard]] int8_t pop_rotary();
    
    void set_a_mask(uint8_t mask);
    void set_b_mask(uint16_t mask);
    
    [[nodiscard]] sw_bank_a get_raw_a();
    [[nodiscard]] sw_bank_b get_raw_b();
}

#endif