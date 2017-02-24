Adafruit USB Serial RGB LCD Backpack
====================================

* KEYWORDS: Adafruit, Backpack, USB, serial, LCD, RGB

## Summary

This code provides fixes and enchancements for the Adafruit USB Serial RGB LCD Backpack.

### BackpackPlus

This code is a complete rewrite of the Adafruit RGB Backpack code. This code adds GPIO, specifically input capability to the Backpack functionality. This allows the LCD to function as a simple remote terminal with up to 4 "key/button" inputs and/or "LED indicators" able to communicate over a simple 2 wire serial interface. The code also incorporates a number of other enhancements detailed below.

See *BACKPACK_PLUS_README.md* for details.

### Adafruit_USBSerialLCDwithIO

This code adds GPIO, specifically input capability to the existing Adafruit code. These changes allow the LCD to function as a simple remote terminal with up to 4 "key/button" inputs and communicate over a simple 2 wire serial interface. It also makes a few fixes noted below.

See *WITH_IO_README.md* for details.


### Adafruit_USBSerialLCD

Adafruit's code downloaded from Backpack as purchased.

See *[https://learn.adafruit.com/usb-plus-serial-backpack/overview]* for details.

### ClearEEPROM

Code to erase the contents of EEPROM. Useful for reinitializing the EEPROM before upload of BackpackPlus code. Load ClearEEPROM.hex. After loading reset and send any character by USB to execute the erase operation. (The lcd.py script can be used to send text such as "x".) Then update BackpackPlus firmaware. For a failed erase, the EEPROM contents will be dumped to the USB serial stream.

### Adafruit Upgrade Requests

The BackpackPlus code utilizes all of the available capacity of the AT90USB162 processor, literally only having a few bytes of free code space. (Actually, the Teensy 1.0 core reports the bootloader starts at 0x3E00, 15,872 bytes) but the interface will not load files larger than 0x3000, 12,228 bytes, in size.) It would nice to have an upgraded Backpack. A number of other features could be added with more memory.

Should Adafruit decide to upgrade the Backpack, it would be nice to include:

* **ATMEGA32U4 or other non-obsolete processor**: This would provide more memory for code, RAM, and EEPROM, with only minor *#define* changes the Backpack could easily support 40x4 or 20x6 displays, although Adafruit does not presently offer any such dsiplays. (The Adafruit schematics actually call out the ATMEGA32U4 part but the Backpack uses the AT90USB162, so I assume they are pin compatible.)

* **More I/O**: its there just not used.

* **4-pin JST connector**: To support both RX and TX. 5-pins with a spare line for hacking would be better.

## My Compiling and Upload Procedure

### IMPORTANT NOTE

First recommendation for those wishing to use this code, get the Backpack working with an LCD and communicating over USB before attempting code changes so you won't have to debug hardware and code/programming issues simultaneously!

### Testing

Code has been tested with an Adafruit 16x2 RGB Negative LCD. I currently do not have a 20x4 display.

### TeensyPlus Core

This folder provides a modified copy of the Teensy 1.0 core for archival since the vendor no longer supports this version. The Print.h Print.cpp files have been modified for this version to include the simple print formatted (i.e. spf) function used by BackpackPlus code.

### Compiling

Initially, I setup a virtual machine running Arduino 1.05 on Windows 7 in order to get the existing code compiled. Note: the code requires at least Arduino 1.0, and PRJC discontinued Teensy 1.0 support with Arduino 1.06. Using the Arduino IDE process converts a "\*.ino" file into a temp "\*.cpp" file that can be directly compiled with the AVR compiler.

I copied the temp file to the code directory, along with a copy of the Arduino libraries and modified Teensy 1.0 core into the build directory. Then I created a "brute force" *makefile* to compile and link the code directly with the avr-gcc/avr-g++ version 4.3.3 compilers. The *makefile* could certainly be greatly simplified and improved, but suffices for this single build operation. Output can be found in the tmp folder under the build folder. 

#### *make* Commands

**make**: Directly builds the BackpackPlus code to a hex file

**make clean**: Deletes all the code files, object files, and such in the tmp folder for a clean build.

**make list**: Lists the temp directory

**make burn**: Uploads the code to the Backpack using AVRDUDE. Reset the Backpack to enter bootloader mode (red LED flashes). Then run *make build* command.

**make flash**: Download the code from the Backpack using AVRDUDE.Reset the Backpack to enter bootloader mode (red LED flashes). Then run *make flash* command. Code placed in temp folder as flash.hex

**make eeprom**: Download the EEPROM data from the Backpack using AVRDUDE. then dumps to console using hex2Txt.py script. Reset the Backpack to enter bootloader mode (red LED flashes). Then run *make eeprom* command. EEPROM hex file placed in temp folder as ee.hex


**TBD**

### Download Code and EEPROM/Uploading Code

AVRDUDE can talk directly to the Backpack to upload and download code and EEPROM data. The following commands represent what I used for various tasks. These would have to be adjusted for your particular setup, mainly identifying the correct COM port. The commands program the part using the USB port. If using other programming hardware the commands would need to be corrected for that as well. See the AVRDUDE manual for more information.

**NOTE**: The Backpack presents 2 different ports depending on whether it is operating as a display or running in bootloader (programming) mode. The ports can be determined using *Device Manager* on a Windows system or listing the /dev directory on a Linux system. The easiest method is to determine the connected devices, then attach the LCD, and note the "new" port added. Then "reset" the LCD and again note the new port. Depending on the flavor of your OS, you may need to install a Virtual COM port driver.

#### Commands

To read EEPROM...

**avrdude -P COM5 -c AVR109 -p usb162 -U eeprom:r:eeprom.hex:h**

To read FLASH...

**avrdude -P COM5 -c AVR109 -p usb162 -U flash:r:original.hex**

To write FLASH...

**avrdude -P COM5 -c AVR109 -p usb162 -U flash:w:BackpackPlus.hex**

## Python (2.7.x) Support Code

### lcd.py

The **lcd.py** script implements a command interpretor for initializing display settings. It supports all commands, including newly defined commands as well as obsolete Adafruit only commands, and can directly send text too. Commands can be specified from the commandline (-c) or entered interactively (-i) or read from a commandfile (-f). It dumps any received data in response to commands to the console.

Usage: **python lcd.py [-h] [-p <port>] -i | -f commandfile | -c <command> <parameters>] | "<text>"**

For a list of LCD commands and parameters as well as usage syntax use **python lcd.py -h** or just **python lcd.py**

The file **lcd_init.txt** contains an example definition for configuring a new LCD module, default as 16x2 display. This file will work for 20x4 LCD displays as well by simply changing the SetSize command. It can be customized for other features and setting as desired too.

**NOTE: The lcd.py script requires installation of the PySerial module.**

### Encoded text strings  
    
The **lcd.py** script supports encoded text strings making the substitutions of the table before sending text to the LCD. This means strings such as "Water temp: 75\2\n" will be sent to the display as 16 characters: str("Water temp: 75")+chr(2)+chr(10).

TEXT | CHAR | DESCRIPTION
------ | ---- | -----------
"\0" |  0 | Custom character 0
"\1" |  1 | Custom character 1
"\2" |  2 | Custom character 2
"\3" |  3 | Custom character 3
"\4" |  4 | Custom character 4
"\5" |  5 | Custom character 5
"\6" |  6 | Custom character 6
"\7" |  7 | Custom character 7
"\b" |  8 | Backspace
"\n" | 10 | Linefeed (Linux newline)
"\r" | 14 | Carriage Return (Mac newline)

### Custom Character Definitions

The *lcd_init.txt* commandfile contains definitions for some common custom characters. Character bank 0 gets loaded by default on power up. The handy utility at [http://mikeyancey.com/hamcalc/lcd_characters.php]

BANK | CHAR | DESCRIPTION
------ | ---- | -----------
  |   | Special symbols
0 | 0 | Newline/return symbol
0 | 1 | Elipses (...)
0 | 2 | Temperature degrees symbol
0 | 3 | +/- symbol
0 | 4 | Up arrow
0 | 5 | Down Arrow
0 | 6 | Right Arrow
0 | 7 | Left Arrow
  |   | Adafruit Horizontal Bars
1 | 0 | Minimum
1 | 1 | 
1 | 2 | 
1 | 3 | 
1 | 4 | 
1 | 5 | 
1 | 6 | 
1 | 7 | Maximum
  |   | Adafruit Vertical Bars
2 | 0 | Minimum
2 | 1 | 
2 | 2 | 
2 | 3 | 
2 | 4 | 
2 | 5 | 
2 | 6 | 
2 | 7 | Maximum
  |   | Battery charge levels
3 | 0 | Empty
3 | 1 | ~1/8 charge
3 | 2 | ~1/4 charge
3 | 3 | ~3/8 charge
3 | 4 | ~1/2 charge
3 | 5 | ~5/8 charge
3 | 6 | ~3/4 charge
3 | 7 | Full change

### hex2txt.py

A script to dump a hex file as both hex and readable ASCII for an organized visual display of EEPROM data from AVRDUDE. Handy for reviewing internal settings. Use AVRDUDE to dump the EEPROM to a hex file, for example:

**avrdude -p usb162 -P COM7 -c avr109 -U eeprom:r:eeprom.hex:h**

Usage: **python hex2Txt.py eeprom.hex**
