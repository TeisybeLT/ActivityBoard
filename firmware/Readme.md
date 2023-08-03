# Firmware for the car radio faceplate

## Requirements for building and flashing the firmware

Building:

* GNU make
* avr-gcc 12.2 or newer

Flashing:

* avrdude
* USBasp programmer

## How to build the firmware

1. Open your preferred terminal
1. Navigate to `firmware` directory (the one this readme file resides)
1. Type `make`

This will build the firmware. Flashable file located in `firmware/bin/radio_faceplate.elf`.

You can use `make clean` to remove all build artifacts.

If you need extra debug information, pass `debuguart=true` option when issuing `make` command (`make debuguart=true`). Debugging capabilities are described below.

## Flashing the firmware

1. Disconnect external power
1. Connect USBasp programmer to the AVR
1. Navigate to `firmware` directory
1. Run `make flash`. If the project was not built - it will be built as a part of the flashing step

Currently, only USBasp programmer is supported. If you need to flash this firmware using another programmer, you will need to work out how to flash the `radio_faceplate.elf` using your programmer of choice.

## Debug messages

The firmware has capability to send debug messages via a serial connection. This functionality must be enabled during compilation using a special build flag (see section on building the firmware). This firmware supports one-way communication only (MCU transmits data). Communication speed is set during compilation in the `main.cpp` file. Communication mode is hard-coded as 8 data bytes and 1 stop byte without any parity bits. To wire up the serial connection to an external device, use the standard Atmega 328p TXD pin (pin 3 on 28SPDIP package).

A convenience macro is provided in the `debug.hpp` file which wraps `printf`, but is only compiled in when debug messages are enabled.

Please note that if you are utilizing debug messages heavily, keep in mind memory capabilities of the device.
