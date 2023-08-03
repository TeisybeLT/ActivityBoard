#include "uart.hpp"

int uart::putc(char ch, ::FILE*)
{
    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = ch;
    
    return 0;
}