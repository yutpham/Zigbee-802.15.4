###############################################################################
# Makefile for the project Zigbee_mac
###############################################################################

## General Flags
PROJECT = Zigbee_mac
MCU = atmega128
TARGET = Zigbee_mac.elf
CC = avr-gcc

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -DF_CPU=8000000  -O0 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -Wp,-M,-MP,-MT,$(*F).o,-MF,dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=Zigbee_mac.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0


## Include Directories
INCLUDES = -I"C:\WinAVR\avr\include\avr" 

## Library Directories
LIBDIRS = -L"C:\WinAVR\avr\lib\avr5" 

## Objects that must be built in order to link
OBJECTS = TestMAC.o 

## Build
all: $(TARGET) Zigbee_mac.hex Zigbee_mac.eep Zigbee_mac.lss

## Compile
TestMAC.o: ../../../../TestMAC.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) Zigbee_mac.elf dep/ Zigbee_mac.hex Zigbee_mac.eep Zigbee_mac.lss Zigbee_mac.map


## Other dependencies
-include $(shell mkdir dep 2>/dev/null) $(wildcard dep/*)

