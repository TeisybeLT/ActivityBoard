#ifndef DISPLAY_SEGMENTS_HPP
#define DISPLAY_SEGMENTS_HPP

#include "display.hpp"
#include "traits.hpp"
#include "utils.hpp"

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
        
        seg_address calculate_seg_offset(seg_address base_segment, uint8_t offset)
        {
            const auto new_bit_offset = static_cast<int8_t>(base_segment.bit - offset);
            if (new_bit_offset < 0)
            {
                return seg_address {
                    .byte = static_cast<uint8_t>((base_segment.byte + (offset * 2 - 1)) & 0b11111), 
                    .bit = static_cast<uint8_t>((new_bit_offset + 8) & 0b111)
                };
            }

            return seg_address {
                .byte = static_cast<uint8_t>((base_segment.byte + (offset * 2)) & 0b11111),
                .bit = static_cast<uint8_t>(new_bit_offset & 0b111)
            };
        }           
        
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
        };
        
        using IndexType = uint16_t;
        using map = segment_map_multi<IndexType, a, b, c, d, e, f, g1, g2, h, il, j, k, m>;
        constexpr IndexType symbols[] = 
        {
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
            0b0000000000000, // Clear
        };
        
        constexpr auto symbol_count = utils::array_size(symbols);
    }
}

#endif