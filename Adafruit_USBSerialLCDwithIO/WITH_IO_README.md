Adafruit USB Serial LCD with IO
================================

* KEYWORDS: Adafruit, USB, serial, LCD, I/O

## Summary

This code adds GPIO, specifically input capability to the existing Adafruit code. These changes allow the LCD to function as a simple remote terminal with up to 4 "key/button" inputs and communicate over a simple 2 wire serial interface. It also makes a few fixes noted below.

## Port Behavior Change

While not receiving any serial data, the backpack *polls* the port pins "PC7 PC4 PC2 PB0", also defined as GPO_4 (pin 16), GPO_3 (pin 19), GPO_2 (pin 20), GPO_1 (pin 8), respectively<sup>1</sup>. It reports *changes* on the serial communications port (TX line and over USB). It returns the port state changes as a hex string "#0x0H\n", where H is a hex value between 0-F representing the port bits. So a keypress that changes a GPIO pin will normally report one line of data for a pin going low and a second line for the pin going back high (or vice versa depending on its default state). Note, for easier integration with existing code (and avoiding interrupts), the code just polls the port, which only provides a crude debounce that could possibly report more than one transistion for a single keypress.

  Note 1. Because of this change, GPIO is now referenced as GPIO.

## Other Changes 

**Splash Screen**: Fixed EEPROM "pointer" for storing and retrieving splash screen data, which incorrectly mangles the data addressing. Creating a splash screen will overwrite stored custom characters and possibly other settings.

**UART RX Pullup**: Fixed the RX input to have a pullup. The Adafruit code drives the UART RX line high (for noise immunity) instead of just applying a pullup.

**UART Instance**: The UART instance is not needed, as already instanced in HardwareSerial as Serial1. A *#define* replaces the instance reference.

**GPIO EEPROM Addresses**: Not specified as contiguous, but used contiguously since all reference GPIO_1 address.

**getContrast function**: Missing return statement, gives compiler warning/error.

**Splash Storage Indexing**: 

## Other Backpack Issues (not fixed)

**Carriage Return and Newline Swap**: Application note [https://learn.adafruit.com/usb-plus-serial-backpack/sending-text](https://learn.adafruit.com/usb-plus-serial-backpack/sending-text) says code ignores carriage returns, '\r' or 0x0D character, and a newline, '\n' or 0x0A, fills current line with blanks and moves to the next line, but the code actually handles it backwards. Probably not noticed since most applications probably send '\r\n' making it appear correct.

**TX and RX Lines**: Note, pin 3 of CN3 is incorrectly labeled on the board silkscreen as TX when it actually (correctly) connects to RX. This may be confusing when wiring to the separate UART TX and RX pins, which are labled correctly.

**Custom Character Banks**: Code actually accepts 5 (i.e. 0-4) custom character banks vs the documented 4.

**Splash Screen**: Defining a splash screen larger than 20x2 (40 characters) will overrun RX buffer because of the EEPROM write time.

**Timing**: Not an issue for this code, but the unexplained/unneeded timer register settings in displayOn routine (maybe from a leftover attempt at timing the display on-time) seem to impact the millis() function making it act like micros()???.

## Command Changes

### GPIO Read (0xFE 0x55 &lt;mask&gt;)

Sets the read mask and performs a read of the port state, adjusted with mask. Returns same hex string format as polling reports, #0x0H, where H represents to masked pin states. 

#### Mask

The low nibble of **mask** ANDs with the 4 GPIO pins (GPIO[4:1]) and the high nibble XORs with the AND result, effectively inverting the specified bits. Therefore, the mask sets which bits (GPIO[4:1]) are watched for change and which bits have inverted sense. For example a 0xFF mask watches all bits all with inverted sense. A mask of 0x31 watches only the 2 LSBs (GPIO[2:1]) without inverted sense only for the LSB. The unset read mask defaults to 0xFF.

### GPIO Off (0xFE 0x56 &lt;bit&gt;)

Name change only, i.e. GPIO to GPIO.

### GPIO On (0xFE 0x57 &lt;bit&gt;)

Name change only, i.e. GPIO to GPIO.

### GPIO Start State (0xFE 0xC3 &lt;bit&gt; &lt;mode&gt;)

Extends existing command to include input modes. Sets the pin mode for a given port bit and saves in EEPROM as the GPIO power on state, where bit is the GPIO bit 1-4 and mode is 0x00: OUTPUT LOW, 0x01: OUTPUT HIGH, 0x10: INPUT, 0x11: INPUT_PULLUP (default). Input states represent new capability. Since an unprogrammed EEPROM returns 0xFF, a new uncommanded part defaults to INPUT_PULLUP.

**Note: GPIO defaults to a "safe" INPUT_PULLUP, where original Adafruit GPO defaults to OUTPUT LOW, but the user can easily change this using the GPIO Start State command.**

## Web Tutorial Command Reference Comments

ref: [https://learn.adafruit.com/usb-plus-serial-backpack/command-reference](https://learn.adafruit.com/usb-plus-serial-backpack/command-reference)

  - **"Moving and changing the cursor"** heading appears at end of Change startup splash screen bullet

  - **"Display On - 0xFE 0x46 - turn the display backlight off"** should read 
  **"Display Off - 0xFE 0x46 - turn the display backlight off"**

  - **"Cursor forward - 0xFE 0x4D - move cursor back one space, ..."** should read 
  **"Cursor forward - 0xFE 0x4D - move cursor forward one space, ..."**
  
  - **"Set RGB Backlight Color"** indented more than other commands

  - **"Custom Characters"** heading appears at end of **"Set LCD size"** bullet, with extra space to next bullet

  - Since commands have variable number of parameters, it would be helpful to add placeholders for extra command bytes, such as,
  
  **Set cursor position - 0xFE 0x47 &lt;row&gt; &lt;col&gt;**
  
  - More complete compile and upload details would be helpful, as Teensy 1.0 is no longer supported.
  
  
