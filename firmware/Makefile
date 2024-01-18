TARGET = radio_faceplate
DEVICE = atmega328p
CLOCK = 8000000

PROGRAMMER = USBasp

CC = avr-gcc
CXX = avr-g++
LD = avr-gcc
OBJCOPY = avr-objcopy
SIZE = avr-size

CFLAGS = -std=c23
CXXFLAGS = -std=c++23 -fno-exceptions -fno-rtti
CPPFLAGS = -flto -Wall -Wextra -Werror -Wshadow -Wconversion -Wunreachable-code -mmcu=$(DEVICE) -DF_CPU=$(CLOCK) -Os --param=min-pagesize=0
LDFLAGS = -flto -mmcu=$(DEVICE) -Wl,-Map=bin/$(TARGET).map,--cref

BIN_DIR = bin
OBJ_DIR = $(BIN_DIR)/obj

SRC = main.cpp display.cpp display_segments.cpp input.cpp buzzer.cpp heap.cpp demo_screen.cpp match_screen.cpp screen_manager.cpp
OBJ = $(SRC:%=%.o)

ifeq ($(debuguart), true)
	CPPFLAGS += -DDEBUG_UART
	SRC += uart.cpp
endif

all: dirs $(TARGET).elf size

dirs:
	mkdir -p $(OBJ_DIR)

%.cpp.o: %.cpp
	$(CXX) -c $^ $(CPPFLAGS) $(CXXFLAGS) -o $(OBJ_DIR)/$@

%.c.o: %.c
	$(CC) -c $^ $(CPPFLAGS) $(CFLAGS) -o $(OBJ_DIR)/$@
	
$(TARGET).elf: $(OBJ)
	$(LD) $(addprefix $(OBJ_DIR)/,$(OBJ)) -o $(addprefix $(BIN_DIR)/,$@) $(LDFLAGS) 

size: $(TARGET).elf
	$(SIZE) --format=avr --mcu=$(DEVICE) bin/$(TARGET).elf

flash: all
	avrdude -c $(PROGRAMMER) -p $(DEVICE) \
		-U flash:w:$(BIN_DIR)/$(TARGET).elf:e \
		-U lfuse:w:$(BIN_DIR)/$(TARGET).elf:e \
		-U hfuse:w:$(BIN_DIR)/$(TARGET).elf:e \
		-U efuse:w:$(BIN_DIR)/$(TARGET).elf:e
	
clean: $(DIRS)
	rm -rd $(BIN_DIR)

.PHONY: all clean dirs flash
