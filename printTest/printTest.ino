//Test of adding simple spf (spf) function to Teensy core
/****
simple print format (i.e. printf-like, with simpler syntax)
spf written to handle memory constraints of AT90USB162

usage: 
  Serial.spf(F("format_string"), arg1, ...);

fmt metadata form: %[#][pad][width]specifier
  where ...
    # reads a count argumet
      for 'c' it specifies the number of times to repeat the character
      for 's' it specifies the number of characters to read (i.e. act like a char buf)
      ignored for numbers
    pad:0 or '' for default space
      numbers right justified, strings only left justified, ignored for char
    width: 1-9 only (i.e. a single digit) or '' for no padding
    specifier is one of the case-insensitive characters: 'c', 'i', 's', 'x'
      all other characters default to 'i'
      x (hex) always printed uppercase, case ignored for all others
  NOTE: fmt may include \ escape sequences handled by c-parser when storing F("") strings
****/

#include <EEPROM.h>
#include <util/delay.h>

// code parameters...
#define VERSION "1702v1"                // YEAR+MONTH+'v'+VERSION 

#define USB Serial                      // Rename Serial for code clarity
#define UART Serial1                    // Hardware Serial already defines Serial1 instance
#define DEBUGTERM Serial                // Select which terminal used for debug
#define CR 13                           // Carriage Return character
#define LF 10                           // Line Feed character
#define BS 8                            // Backspace character

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

// follow Arduino style setup and loop functions...
void setup() {
  USB.begin(9600);
  UART.begin(9600);
}

// basic loop operation:
void loop() {
  char tmp;
  uint8_t a,b,c,d;
  // look for serial data input
  if (UART.available()) {
    tmp = UART.read();
    UART.print(tmp,DEC);
    UART.print(' ');
    UART.print(tmp,HEX);
    UART.println();
    USB.print(tmp,DEC);
    USB.print(' ');
    USB.print(tmp,HEX);
    USB.println();
  }; 
  if (USB.available()) {
    while (USB.available()) {
      tmp = USB.read();
      USB.print(tmp,DEC);
      USB.print(' ');
      USB.print(tmp,HEX);
      USB.println();
    }
    
    a = 65;
    b = 66;
    c = 67;
    d = 33;
    
    char text[] = "xyz?";
    int n = 0;
    
    n = USB.spf(F("spf test of escape:\\ \"1,\t2,3,%%,4\"\r\n")); // c-parser automatically handles escapes
    USB.print(n); USB.println(" chars printed!");
    _delay_ms(10);
    n = USB.spf(F("spf test of %%c: %c,%c,%c,%c\n"), a, b, c, d);
    USB.print(n); USB.println(" chars printed!");
    _delay_ms(10);
    n = USB.spf(F("spf test of %%c: %c,%c,%c,%c\n"), EEPROM.read(0), EEPROM.read(1), EEPROM.read(2), EEPROM.read(3));
    USB.print(n); USB.println(" chars printed!");
    _delay_ms(10);
    n = USB.spf(F("spf test of %%s: %s,%s,%s,%s\n"), text, "dvc", "yes", F("PROGRAM MEMORY"));
    USB.print(n); USB.println(" chars printed!");
    _delay_ms(10);
    USB.print(n); USB.println(" chars printed!");
    n = USB.spf(F("spf test of %%s: %s,%s,%s,%s\n"), "a", "b", "c", "d");
    USB.print(n); USB.println(" chars printed!");
    _delay_ms(10);
    n = USB.spf(F("spf test of %%#s: '%5s,%4s,%3s,%2s'\n"), "ax", "bx", "cx", "dx");
    USB.print(n); USB.println(" chars printed!");
    _delay_ms(10);
    
    a = EEPROM.read(0);
    b = EEPROM.read(1);
    c = EEPROM.read(2);
    d = EEPROM.read(3);
    
    n = USB.spf(F("spf test of %%x: %x,%x,%x,%x\n"), a, b, c, d);
    USB.print(n); USB.println(" chars printed!");
    _delay_ms(10);
    n = USB.spf(F("spf test of %%0x: %0x,%0x,%0x,%0x\n"), a, b, c, d);
    USB.print(n); USB.println(" chars printed!");
    _delay_ms(10);
    n = USB.spf(F("spf test of %%0#X: %02X,%04X,%08X,%08X\n"), a, b, c, d);
    USB.print(n); USB.println(" chars printed!");
    _delay_ms(10);
    n = USB.spf(F("spf test of 0x%%02X: 0x%02X,0x%02X,0x%02X,0x%02X\n"), a, b, c, d);
    USB.print(n); USB.println(" chars printed!");
    _delay_ms(10);
    n = USB.spf(F("spf test of %%#i: %2i,%4i,%6i,%8i\n"), a, b, c, d);
    USB.print(n); USB.println(" chars printed!");
    _delay_ms(10);
    n = USB.spf(F("spf test of %%d: %d,%d,%d,%d\n"), a, b, c, d);
    USB.print(n); USB.println(" chars printed!");
     _delay_ms(10);
   n = USB.spf(F("spf test of *%%8d: %8d,%8d,%8d,%8d\n"), -1, a, (int)10000, 1024000);
    USB.print(n); USB.println(" chars printed!");
    _delay_ms(10);
    n = USB.spf(F("spf test of %%#d: %0d,%2d,%4d,%06\n"), a, b, c, d);
    USB.print(n); USB.println(" chars printed!");
    _delay_ms(10);
    n = USB.spf(F("spf test of %%2.1f: %2.1f,%2.1f,%2.1f,%2.1f\n"), a, b, c, d);
    USB.print(n); USB.println(" chars printed!");
    _delay_ms(10);
    n = USB.spf(F("spf test of %%05.1f: %05.1f,%05.1f,%05.1f,%05.1f\n"), a, b, c, d);
    USB.print(n); USB.println(" chars printed!");
    
        
    uint8_t x = 10;
    uint8_t y = 2;
    uint8_t z = 0;
    char arr[2][27] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZ","abcdefghijklmnopqrstuvwxyz"};
    char charr[2][10];
    for (uint8_t i=0;i<y;i++)
      for (uint8_t j=0;j<x;j++)
        charr[i][j] = (char)(65+j+32*i);
    
    z += USB.spf(F("%#c\n"),x+2,'+');
    for (uint8_t i=0; i <y; i++)
      z += USB.spf(F("+%#s+\n"),x,arr[i]);
    z += USB.spf(F("%#c\n"),x+2,'+');
    USB.spf(F("Printed %i chars!\n"),z);
    
    z += USB.spf(F("%#c\n"),x+2,'-');
    for (uint8_t i=0; i <y; i++)
      z += USB.spf(F("|%#s|\n"),x,charr[i]);
    z += USB.spf(F("%#c\n"),x+2,'-');
    USB.spf(F("Printed %i chars!\n"),z);
    
    z += USB.spf(F("%#c\n"),x+2,'-');
    USB.spf(F("Printed %i chars!\n"),z);
    
  }; 
} 

