#ifndef DISPLAY_SEGMENTS_HPP
#define DISPLAY_SEGMENTS_HPP

#include "display.hpp"

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
}

#endif