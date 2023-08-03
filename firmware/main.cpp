
#include <avr/fuse.h>

FUSES =
{
    // Calibrated internal RC oscillator, default start up time
    .low = FUSE_CKSEL0 & FUSE_CKSEL2 & FUSE_CKSEL3 & FUSE_SUT0,

    // Defaults
    .high = HFUSE_DEFAULT,
    .extended = EFUSE_DEFAULT 
};

int main()
{
    while(true)
    {}

    return 0;
}