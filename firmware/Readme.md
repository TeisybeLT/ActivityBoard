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

## Flashing the firmware

1. Disconnect external power
1. Connect USBasp programmer to the AVR
1. Navigate to `firmware` directory
1. Run `make flash`. If the project was not built - it will be built as a part of the flashing step

Currently, only USBasp programmer is supported. If you need to flash this firmware using another programmer, you will need to work out how to flash the `radio_faceplate.elf` using your programmer of choice.
