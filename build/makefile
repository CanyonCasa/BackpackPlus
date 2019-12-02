# BackpackPlus Makefile...

VPATH = ./tmp:./teensyplus:./libraries:../BackpackPlus
TEMP = ./tmp
PORT = COM4
AVRDUDE_MODE = AVR109
DEVICE = usb162
FLASH_HEX = flash.hex
EE_HEX = ee.hex
SCRIPTS = ../PythonScripts

CORE = ./teensyplus
CORE_C_SRC =  ./teensyplus/%.c
CORE_CPP_SRC =  ./teensyplus/%.cpp
CORE_ARCHIVE = core.a
CORE_C_OBJECTS = keylayouts.o malloc.o pins_teensy.o usb.o WInterrupts.o wiring.o
CORE_CPP_OBJECTS = HardwareSerial.o IPAddress.o main.o new.o Print.o Stream.o Tone.o usb_api.o WMath.o WString.o

EEPROM_LIB = ./libraries/EEPROM
EEPROM_SRC = EEPROM/EEPROM.cpp
EEPROM_OBJ = EEPROM.o
LCD_LIB = ./libraries/LiquidCrystal
LCD_SRC = LiquidCrystal/LiquidCrystal.cpp
LCD_OBJ = LiquidCrystal.o
LIB_FOLDER = ./libraries
LIBRARIES = LiquidCrystal EEPROM 
LIB_OBJECTS = LiquidCrystal.o EEPROM.o

CODE = BackpackPlus
CODE_SRC = ../BackpackPlus/BackpackPlus.cpp
CODE_OBJ = BackpackPlus.o
CODE_HEX = BackpackPlus.hex

CC = avr-gcc
CC_OPTIONS = -c -g -Os -Wall -ffunction-sections -fdata-sections \
  -mmcu=at90usb162 -DF_CPU=16000000L -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=105 \
  -DUSB_SERIAL -DLAYOUT_US_ENGLISH
CC_INCLUDES = -I$(CORE)
CPP = avr-g++
CPP_OPTIONS = -c -g -Os -Wall -fno-exceptions -ffunction-sections -fdata-sections \
  -mmcu=at90usb162 -DF_CPU=16000000L -MMD -DUSB_VID=null -DUSB_PID=null -DARDUINO=105 \
  -felide-constructors -std=c++0x -DUSB_SERIAL -DLAYOUT_US_ENGLISH
LIB_INCLUDES = $(addprefix $(LIB_FOLDER)/,$(LIBRARIES))
CPP_INCLUDES = $(addprefix -I,$(CORE) $(LIB_INCLUDES))

AR = avr-ar
AR_OPTIONS = rcs

LINK = avr-gcc
LINK_FILES = BackpackPlus.elf BackpackPlus.o LiquidCrystal.o EEPROM.o core.a
LINK_OPTIONS = -Os -Wl,--gc-sections,--relax -mmcu=at90usb162 -o $(addprefix $(TEMP)/,$(LINK_FILES)) -L$(TEMP) -lm

COPY = avr-objcopy
COPY_OPTIONS = -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load --no-change-warnings --change-section-lma .eeprom=0
COPY_FILES = BackpackPlus.elf BackpackPlus.eep
COPY_OPTIONS2 = -O ihex -R .eeprom 
COPY_FILES2 = BackpackPlus.elf BackpackPlus.hex

# Linking and building the hex file output
$(CODE) : $(CODE_OBJ) $(LIB_OBJECTS) $(CORE_ARCHIVE)
	$(LINK) $(LINK_OPTIONS)
	$(COPY) $(COPY_OPTIONS) $(addprefix $(TEMP)/,$(COPY_FILES))
	$(COPY) $(COPY_OPTIONS2) $(addprefix $(TEMP)/,$(COPY_FILES2))

# Code compiling...
$(CODE_OBJ) : $(CODE_SRC) $(LIB_OBJECTS)
	$(CPP) $(CPP_OPTIONS) $(CPP_INCLUDES) $(CODE_SRC) $(addprefix $(TEMP)/,$(LIB_OBJECTS)) -o $(TEMP)/$@

# Libraries...
$(EEPROM_OBJ) : $(EEPROM_SRC)
	$(CPP) $(CPP_OPTIONS) $(CPP_INCLUDES) $^ -o $(TEMP)/$@
$(LCD_OBJ) : $(LCD_SRC)
	$(CPP) $(CPP_OPTIONS) $(CPP_INCLUDES) $^ -o $(TEMP)/$@

# Core archive build...
$(CORE_ARCHIVE) : $(CORE_C_OBJECTS) $(CORE_CPP_OBJECTS)
	$(AR) $(AR_OPTIONS) $(TEMP)/$@ $(addprefix $(TEMP)/,$^)

# Core C files...
keylayouts.o : keylayouts.c
	$(CC) $(CC_OPTIONS) $(CC_INCLUDES) $^ -o $(TEMP)/$@ 
malloc.o : malloc.c
	$(CC) $(CC_OPTIONS) $(CC_INCLUDES) $^ -o $(TEMP)/$@ 
pins_teensy.o : pins_teensy.c
	$(CC) $(CC_OPTIONS) $(CC_INCLUDES) $^ -o $(TEMP)/$@ 
usb.o : usb.c
	$(CC) $(CC_OPTIONS) $(CC_INCLUDES) $^ -o $(TEMP)/$@ 
WInterrupts.o : WInterrupts.c
	$(CC) $(CC_OPTIONS) $(CC_INCLUDES) $^ -o $(TEMP)/$@ 
wiring.o : wiring.c
	$(CC) $(CC_OPTIONS) $(CC_INCLUDES) $^ -o $(TEMP)/$@ 

# Core C++ files...
HardwareSerial.o : HardwareSerial.cpp
	$(CPP) $(CPP_OPTIONS) $(CPP_INCLUDES) $^ -o $(TEMP)/$@
IPAddress.o  : IPAddress.cpp
	$(CPP) $(CPP_OPTIONS) $(CPP_INCLUDES) $^ -o $(TEMP)/$@
main.o  : main.cpp
	$(CPP) $(CPP_OPTIONS) $(CPP_INCLUDES) $^ -o $(TEMP)/$@
new.o  : new.cpp
	$(CPP) $(CPP_OPTIONS) $(CPP_INCLUDES) $^ -o $(TEMP)/$@
Print.o  : Print.cpp
	$(CPP) $(CPP_OPTIONS) $(CPP_INCLUDES) $^ -o $(TEMP)/$@
Stream.o  : Stream.cpp
	$(CPP) $(CPP_OPTIONS) $(CPP_INCLUDES) $^ -o $(TEMP)/$@
Tone.o  : Tone.cpp
	$(CPP) $(CPP_OPTIONS) $(CPP_INCLUDES) $^ -o $(TEMP)/$@
usb_api.o  : usb_api.cpp
	$(CPP) $(CPP_OPTIONS) $(CPP_INCLUDES) $^ -o $(TEMP)/$@
WMath.o  : WMath.cpp
	$(CPP) $(CPP_OPTIONS) $(CPP_INCLUDES) $^ -o $(TEMP)/$@
WString.o : WString.cpp
	$(CPP) $(CPP_OPTIONS) $(CPP_INCLUDES) $^ -o $(TEMP)/$@

# list tmp files...
list:
	$(info VPATH = [${VPATH}])
	ls $(TEMP)

# remove tmp files...
clean:
	rm $(TEMP)/*

# upload hex to Backpack...
burn:
	avrdude -P $(PORT) -c $(AVRDUDE_MODE) -p $(DEVICE) -U flash:w:$(TEMP)/$(CODE_HEX)

# download Backpack code to flash.hex
flash:
	avrdude -P $(PORT) -c $(AVRDUDE_MODE) -p $(DEVICE) -U flash:r:$(TEMP)/$(FLASH_HEX):h

# download Backpack EEPROM data to ee.hex
ee:
	avrdude -P $(PORT) -c $(AVRDUDE_MODE) -p $(DEVICE) -U eeprom:r:$(TEMP)/$(EE_HEX):h
	python $(SCRIPTS)/hex2Txt.py $(TEMP)/$(EE_HEX)