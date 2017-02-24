Adafruit USB Serial LCD with IO and Other Enhancements
======================================================

* KEYWORDS: Adafruit, USB, serial, LCD, I/O

## Summary

This code is a complete rewrite of the Adafruit RGB Backpack code. This code adds GPIO, specifically input capability to the Backpack functionality. This allows the LCD to function as a simple remote terminal with up to 4 "key/button" inputs and/or "LED indicators" able to communicate over a simple 2 wire serial interface. The code also incorporates a number of other enhancements detailed below.

## Code Changes Overview Notes

This following describe the basic aspects of code changes:

* The *#define* statements have been organized and grouped at the top of the code.
  - More defines have been added to remove hardcoded values from the code.
* The EEPROM addressing has been organized for less fragmentation.
  - All individual settings start from address zero. 
  - Character blocks and Splash screen block automatically size down from the top.
* Code reorganization and cleanup to enable other changes.
* Serial Terminals
  - USB (Serial) used for configuration, diagnostics, and duplex LCD communications.
  - UART (Serial1) used only for LCD communications, no diagnostics.
  - DEBUGTERM assigned to USB (Serial) by *#define* for debug commincations.
  - Serial data timeout so command mode always returns to text mode.
* Added a simple print format function (i.e. simple printf-like, spf) to Teensy Print.cpp address lame Arduino printing.
* The Adafruit display buffer notion has been embellished to function as a virtual display with more functionality.
  - All calls (after initial "blink" made directly to the Virtual Display, which affect both the LCD and a respective RAM image.
  - Cleaner diagnostics.
  - Scoll display before or after writing text.
* Splash screen changes, including adjustable delay and saving directly from the virtual display.
* Many more checks, as well as new commands, added to minimize EEPROM wear.
* Functionally separate 'Set' and 'Save' commands for brightness, contrast, and RGB backlight.
* Support for calibrating RGB brightness.
* General purpose inputs added to outputs with masking to select active bits and invert data.
* Support for timed display on state.
* More informative 'blink' message with version; only on LCD display.
* Load custom characters bank 0 as default. With definitions in lcd_init.txt file.
* Included debug features.
* New command codes.
* Python support scripts with encoded text string support. See *Python Support Code* section below.

## Functional Code Changes

**Serial Data Timeout**: Serial communications will timeout if not provided with expected parameters so that LCD always returns to text mode. This may have unintended consequences, but prevents the display from hanging.

**Splash Screen and Delay**: The Splash screen gets saved direct from the virtual display (i.e. what's visible on LCD). The splash screen timeout delay can also be set in ~0.1s increments up to 25.5s. The initial 'blink' screen appears for ~1/10 of the splash screen time. Note: the LCD is essentially non-functional until the splash screen timeout has completed.

**New Scroll Mode**: The Adafruit autoscrolling behaves as expected on a large screen, but for a 2-line display this results in one line of usage as the last line will always be blank waiting for the next line. Scrolling has been enhanced to update either before writing the next line or after the previous line (Addfruit mode). For the "before next line" case all lines of the display are preserved for use but the cursor (if turned on) will display at the beginning of the first line. For the "after previous line" case the display behaves the same as Adafruit behavior, which preserves the cursor position at the start of the last line, but essentially reduces the number of display lines by 1. Note: The Adafruit mode could function the same as the "before next line" mode if lines where sent as "\ntext" instead of "text\n", but that is awkward. Use the SaveScrollMode command to define the extended modes.

**Separate Set and Save Commands**: The brightness contrast, and RGB backlight have separate Set and Save commands. Set commands only change the current state while Save commands change the current state and save to EEPROM. This reduces the EEPROM where and enables "lighting special effects" such as flashing the backlight without causing EEPROM wear.

**RGB Calibration**: A ScaleRGB command can be used to set the relative brightness of each LED color. Adafruit hardcodes these values as 100, 190, and 255, for red, green, and blue, respectively.

**General Purpose Inputs**: The Adafruit Backpack only supports general purpose outputs (consistent with Matrix Orbit displays). GPIORead and GPIOReadSaveMask commands enable definition of general purpose inputs too. See *Port Behavior Change* section below for details. The GPIOStartState command has been embellished to support input modes, defined as:

**Timed Display On-State**: The 0x42 command code now supports dsiplay on-time. A separate command has been added for always on.

**Blink Message**: Defined a more informative 'blink' message with version; only on LCD display. (Doesn't make sense to send baud rate message over serial links if they aren't correctly configured the data wouldn't be received; if the com link works the info is obvious.) The blink message now displays something similar to 

    USB/Ser RGB Bkpk
      9600 8N1 +17B1

**Debug**: Compiler conditionals have been removed so a number of debug items are included using the Debug command.

**New command codes**:
Several new commands and some changes and enhancements to existing codes have been made as outlined in *Command Changes* section and 
*Command Summary Table* below. All existing command codes remian backward compatible, although the names/description may need to change to make sense. Commands that save to EEPROM vs just setting values have been renamed. See below and **lcd.py** help for more parameter details.
  
**UART RX Pullup**: UART Rx line driven high vs just pullup.

**UART Instance**: UART instance not needed, as already instanced in HardwareSerial as Serial1.

**GPIO EEPROM Addresses**: Not specified as contiguous, but used contiguously since all reference GPIO_1 address. Redefined references.

**Carriage Return and Newline**: Supports use of '\r' (0x0D, MAC OS), newline, '\n' (0x0A, Linux) or '\r\n' (DOS/Windows newlines). Behaves same as Adafruit definition, which fills current line to the end with spaces.

**Maximum Display Size**: With just a *#define* change the code accomodates other character displays up to 40x4 or 16x6 (although Adafruit doesn't offer and compatible displays larger than the 20x4 definition).

**Custom Character Banks**: Fixed to 4 (i.e. 0-3) custom character banks with bank 0 loaded by default.

### Port Behavior Change

While not receiving any serial data, the backpack *polls* the port pins "PC7 PC4 PC2 PB0", also defined as GPO_4 (pin 16), GPO_3 (pin 19), GPO_2 (pin 20), GPO_1 (pin 8), respectively [1]. It reports *changes* on the serial communications port (TX line and over USB). It returns the port state changes as a hex string "#0x0H\n", where H is a hex value between 0-F representing the port bits. So a keypress that changes a GPIO pin will normally report one line of data for a pin going low and a second line for the pin going back high (or vice versa depending on its default state). Note, for easier integration with existing code (and avoiding interrupts), the code just polls the port, which only provides a crude debounce that could possibly report more than one transistion for a single keypress.

  Note 1. Because of this change, previously referenced GPO is now referenced as GPIO, which largely just impact command descriptions.

### Other Backpack Issues (not fixed)

**TX and RX Lines**: Note, pin 3 of CN3 is incorrectly labeled on the board silkscreen as TX when it actually (correctly) connects to RX. This may be confusing when wiring to separate TX and RX pins, which are labeled correctly.

## Command Changes

These descriptions use the **lcd.py** script command names. Codes and parameters have been included for completeness. The **lcd.py** script help (-h) also provides description and parameter details for all commands. These command changes apply only to the *BackpackPlus* code.

### SaveSplash (0xFE 0x40)

Saves the text visible on the display screen to the EEPROM as splash screen.

### SplashDelay (0xFE 0x41 &lt;splash_delay&gt;)

Sets the splash screen delay time, in ~0.1s increments up to 25.5s, before the display blanks ready for use.

### DisplayOnTimed (0xFE 0x42 &lt;on_time&gt;)

Turns the display on for a set time interval. The Matrix Orbital specs the delay in minutes. For more fine grain resolution the MSB has been defined to specify seconds. This means values 0 to 127 specify minutes (~2 hours max) same as Matrix parts, while values of 128 to 255 specify 0 to 127 seconds (~2 minutes max). A value of zero turns the display on indefinately. Same as Adafruit DisplayOn command, but supports timeout value. See also the new DisplayOn command.
 
### DisplayOn (0xFE 0x45)

Turn display on (indefinately) with no timeout parameter. Behaves like Adafruit command, but has no time parameter. See also the new DisplayOnTimed command.

### SetContrast (0xFE 0x50 &lt;value&gt;)

Set backlight contrast, not saved to EEPROM. Use SaveContrast to also write EEPROM.

### GPIOReadSaveMask (0xFE 0x55 &lt;mask&gt;)

Sets the read mask and performs a read of the port state, adjusted with mask. Returns same hex string format as polling reports.

Low nibble of **mask** ANDs with the 4 GPIO pins (GPIO[4:1]) and the high nibble XORs with the AND result, effectively inverting the specified bits. Therefore, the mask sets which bits (GPIO[4:1]) are watched for change and which bitsd have inverted sense. For example a 0xFF mask watches all bits with inverted sense. A mask of 0x01 watches only the LSB without inverted sense. Read mask default is 0xFF. See *Port Behavior Change* section for more info.

### GPIORead (0xFE 0x59)

Reads the port state, adjusted with the predefined mask. See *GPIOReadSaveMask* command and *Port Behavior Change* section for more info.

### SetBrightness (0xFE 0x99 &lt;value&gt;)

Set backlight brightness, not saved to EEPROM. Use SaveBrightness to also write EEPROM.

### GPIO Start State (0xFE 0xC3 &lt;bit&gt; &lt;mode&gt;)

Extends existing command to include input modes. Sets the pin mode for a given port bit and saves in EEPROM as GPIO power on state, where bit is the GPIO bit 1-4 and mode is 0x00: OUTPUT LOW, 0x01: OUTPUT HIGH, 0x10: INPUT, 0x11: INPUT_PULLUP (default). Input states represent new capability. Since an unprogrammed EEPROM returns 0xFF, a new uncommanded part defaults to INPUT_PULLUP. This likely only needs to be defined once for a given application.

MODE | Description
---- | ----
0x00 | Output Low
0x01 | Output high
0x10 | Input (not supported by Adafruit)
0x11 | Input pullup (default, not supported by Adafruit)

**Note: GPIO defaults to "safe" INPUT_PULLUP, where original GPO defaults to OUTPUT LOW, but the user can easily change this using the GPIO Start State command.**

### SaveRGB (0xFE 0xD0 &lt;red&gt; &lt;green&gt; &lt;blue&gt;)

Sets the backlight red, green, and blue levels (0-255) and saves to EEPROM. Same as Adafruit Set Backlight Color command.

### SaveScrollMode (0xFE 0xD3 &lt;mode&gt;)

Extended scroll modes, saved to EEPROM.

MODE | Description
---- | ----
0 | Autoscroll off
1 | Autoscroll "before next line"
2 | Autoscroll off
3 | Autoscroll "after previous line" (Adafruit default)
    
NOTE: The AutoScrollOff and AutoscrollOn commands still work but only support turning autoscrolling on/off.

### ScaleRGB (0xFE 0xD4 &lt;red&gt; &lt;green&gt; &lt;blue&gt;)

Scales the backlight red, green, and blue LED intensities (0-255) and saves to EEPROM. Hardcoded by Adafruit as 100, 190, and 255 for red, green, and blue levels respectively. This likely only needs to be performed once per display type or design, but can be used to accomodate aging.

### SetRGB (0xFE 0xD5 &lt;red&gt; &lt;green&gt; &lt;blue&gt;)

Sets the backlight red, green, and blue levels (0-255), not saved to EEPROM.

### Debug (0xFE 0xDC &lt;flags&gt;)

Reports debug info based on various flag bits defined in the table below. For example, to enable dumping the virtual display as well as dumping it immediately the flag data is 2^6 + 2^2 = 64 + 4 = 68.

FLAG | Description
---- | ----
0 | Echo characters "USB/UART RX: 'character' [decimal_value] (0x_prefixed_hex_value)\n"
1 | 
2 | Enable debug dump of virtual display data (after every character)
3 | 
4 |
5 |
6 | Dump virtual display data immediately
7 | 

### DumpEEPROM (0xFE 0xDD &lt;start_page&gt; &lt;end_page&gt;)

Dumps/reads (4 byte) pages of EEPROM to serial output streams. Each page written as newline terminated hex text. See code for memory map. 

### EditEEPROM (0xFE 0xDE &lt;page&gt; &lt;byte1&gt; &lt;byte2&gt; &lt;byte3&gt; &lt;byte4&gt;)

Edit/writes/save a (4 byte) page of EEPROM. Useful for small paramter changes. Can be used to write unique user data to unused EEPROM areas, which can be read with the DumpEEPROM command. See code for memory map. 

### Test (0xFE 0xDF &lt;option&gt;)

Reserved for new code testing. 

### Command Summary Table

STATUS | SCRIPT NAME | CODE | DESCRIPTION
--- | --- | --- | ---
ALL | SetBaud | 0x39 | Set serial baud rate and writes to EEPROM.
AF | ChangeSplash | 0x40 | Writes splash screen text to EEPROM.
NEW | SaveSplash | 0x40 | Save the display screen to EEPROM as splash screen.
NEW | SplashDelay | 0x41 | Save and save splash screen display time, ~0.1s units.
ALL | DisplayOnTimed | 0x42 | Same as Adafruit DisplayOn command, but supports timeout value.
NEW | DisplayOn | 0x45 | Turn display on (indefinately) with no timeout.
ALL | DisplayOff | 0x46 | Turn display off.
ALL | SetCursor | 0x47 | Move cursor to (COL,ROW).
ALL | GoHome | 0x48 | Move cursor to home position (1,1).
ALL | UnderlineCursorOn | 0x4A | Turn on the underline cursor.
ALL | UnderlineCursorOff | 0x4B | Turn off the underline cursor.
ALL | CursorBack | 0x4C | Move cursor back one space, wrap to end.
ALL | CursorForward | 0x4D | Move cursor forward one space, wrap to beginning.
ALL | CreateCustomCharacter | 0x4E | Create custom character #0-7 with 8 bytes of data direct to character RAM.
CHG | SetContrast | 0x50 | Set backlight color, not saved to EEPROM. Use SaveContrast to write EEPROM.
ALL | AutoscrollOn | 0x51 | Scroll display lines, where new lines appear at bottom.
ALL | AutoscrollOff | 0x52 | Do NOT scroll display, where new lines overwrite at top.
ALL | BlockCursorOn | 0x53 | Turn on the Block cursor.
ALL | BlockCursorOff | 0x54 | Turn off the Block cursor.
NEW | GPIOReadSaveMask | 0x55 | Save the GPIO mask and read the input bits.
ALL | GPIOOff | 0x56 | Set the general purpose pin 1-4 to LOW (0V).
ALL | GPIOOn | 0x57 | Set the general purpose pin 1-4 to HIGH (5V)
ALL | ClearScreen | 0x58 | Clear text on display
NEW | GPIORead | 0x59 | Read the input bits with the saved mask.
ALL | SaveContrast | 0x91 | Set backlight contrast and save to EEPROM.
ALL | SaveBrightness | 0x98 | Set backlight brightness and save to EEPROM.
CHG | SetBrightness | 0x99 | Set backlight brightness, not saved to EEPROM. Use SaveBrightness to write EEPROM.
ALL | LoadCustomCharacters | 0xC0 | Load characters from EEPROM bank, 0-3 into character RAM.
ALL | SaveCustomCharacter | 0xC1 | Create custom character #0-7 with 8 bytes of data saved to EEPROM bank 0-3.
ALL | GPIOStartState | 0xC3 | Sets the "initial" state of the GPIO pin, default "input pullup".
CHG | SaveRGB | 0xD0 | Sets the backlight red, green, and blue levels (0-255) and saves to EEPROM. Same as Adafruit Set Backlight Color.
ALL | SaveSize | 0xD1 | Set display size up to 20x4, saved to EEPROM. Only needed once.
AF | Testbaud | 0xD2 | Test non standard baudrate. (Adafruit only, not supported).
NEW | SaveScrollMode | 0xD3 | Extended scroll modes, saved to EEPROM.
NEW | ScaleRGB | 0xD4 | Scales the maximum LED intensities red, green, and blue levels (0-255, Adafruit: 100, 190, 255).
NEW | SetRGB | 0xD5 | Sets the backlight red, green, and blue levels (0-255), not saved to EEPROM.
NEW | DEBUG | 0xDC | Controls various debug data.
NEW | DumpEEPROM | 0xDD | Dumps/reads (4-byte) pages of EEPROM.
NEW | EditEEPROM | 0xDE | Edits/writes (save) (4-byte) pages of EEPROM.
NEW | TEST | 0xDF | Reserved for new code development testing.
