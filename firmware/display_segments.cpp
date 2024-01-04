#include "display_segments.hpp"

using namespace display;

seg_address seg_13::calculate_seg_offset(seg_address base_segment, uint8_t offset)
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