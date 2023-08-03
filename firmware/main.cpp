#include "debug.hpp"

#ifdef DEBUG_UART
    #include "uart.hpp"
#endif

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
#ifdef DEBUG_UART
    uart::init_as_stdout<uart::baudrate::B2400>();
#endif

    DEBUG_PRINTF("Init done\r\n");

    while(true)
    {}

    return 0;
}