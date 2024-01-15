#ifndef DISPLAY_SEGMENTS_HPP
#define DISPLAY_SEGMENTS_HPP

#include "display.hpp"
#include "algorithms.hpp"
#include "traits.hpp"
#include "utils.hpp"

#include <string.h>

namespace display
{
    namespace icons
    {
        // Top icons
        constexpr auto stereo = seg_address{0, 0};
        constexpr auto mp3 = seg_address{1, 4};
        constexpr auto af = seg_address{3, 3};
        constexpr auto ta = seg_address{5, 2};
        constexpr auto tp = seg_address{7, 1};
        constexpr auto camera = seg_address{9, 3}; // I don't know what that icon is, it looks like a camera
        constexpr auto pty = seg_address{11, 2};
        constexpr auto eon = seg_address{15, 1};
        constexpr auto loud = seg_address{15, 2};
        constexpr auto casette = seg_address{15, 0};
        constexpr auto swf = seg_address{18, 0};
        constexpr auto loc = seg_address{19, 2};
        constexpr auto reg = seg_address{19, 1};

        // Left side icons
        constexpr auto dsp = seg_address{17, 0};
        constexpr auto cls = seg_address{15, 6};
        constexpr auto pop = seg_address{15, 7};
        constexpr auto rock = seg_address{16, 0};
    }
    
    template<traits::unsigned_integral IndexType, seg_address ... Segments>
        requires (sizeof(IndexType) * 8 >= sizeof...(Segments))
    struct segment_map_base
    {
        static constexpr auto count = static_cast<uint8_t>(sizeof...(Segments));
        static constexpr seg_address map[] = {Segments...};
    };
   
    template<traits::unsigned_integral IndexType, seg_address ... Segments>
    struct segment_map_single : public segment_map_base<IndexType, Segments...>
    {
        static void write_segments(IndexType value)
        {
            using base = segment_map_base<IndexType, Segments...>;

            for (IndexType index = 0; index < base::count; ++index)
            {
                const auto bit = (value >> index) & 0x1;
                if (bit)
                    display::set_segment(base::map[index]);
                else
                    display::clear_segment(base::map[index]);
            }
        }
    };

    namespace seg_7
    {
        constexpr auto a = seg_address{1, 7};
        constexpr auto b = seg_address{3, 6};
        constexpr auto c = seg_address{7, 4};
        constexpr auto d = seg_address{12, 6};
        constexpr auto e = seg_address{13, 1};
        constexpr auto f = seg_address{14, 5};
        constexpr auto g = seg_address{5, 5};
        
        using IndexType = uint8_t;
        using map = segment_map_single<IndexType, a, b, c, d, e, f, g>;
        constexpr IndexType symbols[] = 
        {
            0b0111111, // 0
            0b0000110, // 1
            0b1011011, // 2
            0b1001111, // 3
            0b1100110, // 4
            0b1101101, // 5
            0b1111101, // 6
            0b0000111, // 7
            0b1111111, // 8
            0b1101111, // 9
            0b1110111, // A
            0b1111100, // b
            0b0111001, // C
            0b1011110, // d
            0b1111001, // E
            0b1110001, // F
            0b0000000, // No segment
        };
        
        constexpr auto symbol_count = utils::array_size(symbols);
    }
    
    namespace seg_13
    {
        constexpr auto digit_count = 8;

        constexpr auto a = seg_address{0, 7};
        constexpr auto b = seg_address{1, 5};
        constexpr auto c = seg_address{1, 6};
        constexpr auto d = seg_address{0, 6};
        constexpr auto e = seg_address{0, 2};
        constexpr auto f = seg_address{0, 1};
        constexpr auto g1 = seg_address{0, 5};
        constexpr auto g2 = seg_address{1, 3};
        constexpr auto h = seg_address{0, 4};
        constexpr auto il = seg_address{1, 0};
        constexpr auto j = seg_address{1, 2};
        constexpr auto k = seg_address{1, 1};
        constexpr auto m = seg_address{0, 3};
        constexpr auto decimal = seg_address{10, 7};
        
        seg_address calculate_seg_offset(seg_address base_segment, uint8_t offset);
        
        template<traits::unsigned_integral IndexType, seg_address ... Segments>
        struct segment_map_multi;

        using IndexType = uint16_t;
        using map = segment_map_multi<IndexType, a, b, c, d, e, f, g1, g2, h, il, j, k, m>;
        constexpr IndexType symbols[] = 
        {
            0b0110000000000, // (
            0b1000100000000, // )
            0b0001011000000, // +
            0b0000011000000, // -
            0b1010000000000, // /
            0b1010000111111, // 0
            0b0000000000110, // 1
            0b0000011011011, // 2
            0b0000010001111, // 3
            0b0000011100110, // 4
            0b0100001101001, // 5
            0b0000011111101, // 6
            0b0000000000111, // 7
            0b0000011111111, // 8
            0b0000011100111, // 9
            0b0000011110111, // A
            0b0001010001111, // B
            0b0000000111001, // C
            0b0001000001111, // D
            0b0000001111001, // E
            0b0000001110001, // F
            0b0000010111101, // G
            0b0000011110110, // H
            0b0001000001001, // I
            0b0000000011110, // J
            0b0110001110000, // K
            0b0000000111000, // L
            0b0010100110110, // M
            0b0100100110110, // N
            0b0000000111111, // O
            0b0000011110011, // P
            0b0100000111111, // Q
            0b0100011110011, // P
            0b0000011101101, // S
            0b0001000000001, // T
            0b0000000111110, // U
            0b1010000110000, // V
            0b1100000110110, // W
            0b1110100000000, // X
            0b0000011101110, // Y
            0b1010000001001, // Z
            0b0100100000000, // '\'
            0b0000000001000, // _
            0b0000000000000, // Clear
        };
        
        constexpr auto symbol_count = utils::array_size(symbols);
        
        template<traits::unsigned_integral IndexType, seg_address ... Segments>
        struct segment_map_multi : public segment_map_base<IndexType, Segments...>
        {
            static void write_segments(IndexType value, uint8_t digit)
            {
                using base = segment_map_base<IndexType, Segments...>;

                for (IndexType index = 0; index < base::count; ++index)
                {
                    const auto seg = calculate_seg_offset(base::map[index], digit);
                    const auto bit = (value >> index) & 0x1;
                    if (bit)
                        display::set_segment(seg);
                    else
                        display::clear_segment(seg);
                }
            }
            
            static constexpr uint8_t char_to_idx(char character)
            {
                if (algorithms::in_closed_range('(', ')', character))
                    return uint8_t(character - '(');
                else if (character == '+')
                    return 2;
                else if (character == '-')
                    return 3;
                else if (character == '/')
                    return 4;
                else if (algorithms::in_closed_range('0', '9', character))             
                    return uint8_t(character + 5 - '0');
                else if (algorithms::in_closed_range('A', 'Z', character))
                    return uint8_t(character + 15 - 'A');
                else if (algorithms::in_closed_range('a', 'z', character))
                    return uint8_t(character + 15 - 'a');
                else if (character == '\\')
                    return symbol_count - 3;
                else if (character == '_')
                    return symbol_count - 2;

                return symbol_count - 1; // No character
            }
            
            // TODO: Pre-compute display character indexes at compile time
            // this requires an array and span implementation to store and
            // reference the converted data but will avoid incurring (much)
            // run-time conversion cost for static text
            static void write_text(const char* text, uint8_t at)
            {
                const auto string_length = ::strnlen(text, digit_count);

                auto char_idx = uint8_t{0};
                for (; (at < digit_count) && (char_idx < string_length); ++at)
                {
                    write_segments(symbols[char_to_idx(text[char_idx])], at);                    
                    ++char_idx;
                }
            }

            static void clear_segments()
            {
                static constexpr auto clear_symbol = symbols[symbol_count - 1];

                for (uint8_t idx = 0; idx < digit_count; idx++)
                    write_segments(clear_symbol, idx);
            }
        };

    }
    
    namespace cd_spinner
    {
        constexpr auto a = seg_address{15, 3};
        constexpr auto b = seg_address{15, 4};
        constexpr auto c = seg_address{15, 5};
        constexpr auto center_s_cdp = seg_address{19, 0};
        
        using IndexType = uint8_t;
        using map = segment_map_single<IndexType, a, b, c>;
    }

    namespace animation_wheel
    {
        constexpr auto a = seg_address{16, 7};
        constexpr auto b = seg_address{16, 6};
        constexpr auto c = seg_address{16, 5};
        constexpr auto d = seg_address{16, 4};
        constexpr auto e = seg_address{16, 3};
        constexpr auto f = seg_address{16, 2};
        constexpr auto g = seg_address{16, 1};

        using IndexType = uint8_t;
        using map = segment_map_single<IndexType, a, b, c, d, e, f, g>;
    }

    namespace animation_line
    {
        constexpr auto a = seg_address{17, 7};
        constexpr auto b = seg_address{17, 6};
        constexpr auto c = seg_address{17, 5};
        constexpr auto d = seg_address{17, 4};
        constexpr auto e = seg_address{17, 3};
        constexpr auto f = seg_address{17, 2};
        constexpr auto g = seg_address{17, 1};

        using IndexType = uint8_t;
        using map = segment_map_single<IndexType, a, b, c, d, e, f, g>;
    }
}

#endif