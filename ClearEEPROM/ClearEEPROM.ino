// Program to clear EEPROM
// Useful to restore Backpack when updating firmware
// Load first and clear EEPROM, then update firmware

// After loading reset send any character by USB to execute the erase
// For a failed erase the EEPROM will be dumped

// external libraries...
#include <EEPROM.h>
//#include <util/delay.h>


#define EEPROM_PAGE_SIZE 16             // EEPROM page size used by EXTENDED_DUMP_EEPROM and EXTENDED_EDIT_EEPROM commands
#define EEPROM_SIZE 512                 // EEPROM size
#define EEPROM_PAGES EEPROM_SIZE/EEPROM_PAGE_SIZE
#define USB Serial                      // Rename Serial for code clarity

uint8_t passes = 0;

// follow Arduino style setup and loop functions...
void setup() {
  USB.begin(9600);
}

void loop() {
  uint8_t fails = 0;
  uint8_t x = 0;
  // look for serial data input to trigger erase
  if (USB.available()) {
    while (USB.available())
      x = USB.read();
    passes++;
    USB.println(F("EEPROM erase... "));
    USB.print(F("PASS: "));
    USB.println(passes);
    fails = clearEEPROM(1);
    if (fails) {
      USB.print(F("EEPROM erase Failed!: "));
      USB.println(fails);
      USB.println(F("locations"));
      dumpEEPROM();
    } else {
      USB.print(F("EEPROM erase sucessful!"));
      if (x=='y') 
        dumpEEPROM();
    };
  };
} 

// erase the entire EEPROM for a clean initialization...
uint8_t clearEEPROM(uint8_t passes) {
  uint8_t fails = 0;
  for (uint8_t p=0;p<passes;p++) {                          // make n passes
    fails = 0;
    for (uint16_t i=0;i<EEPROM_SIZE;i++) {                  // make a pass
      eeSave(i,0xFF);
      if (EEPROM.read(i)!=0xFF) fails++;                    // verify
    };
  if (fails==0) break;                                      // quit if successful pass
  };
  return fails;
}

void dumpEEPROM() {
  for (uint8_t p=0;p<EEPROM_PAGES;p++) {
    USB.print(F("EEPROM["));
    USB.print(p,HEX);
    USB.print(F("]: "));
    for (uint8_t j=0;j<EEPROM_PAGE_SIZE;j++) {
      USB.print((j)?',':' ');
      USB.print(EEPROM.read(p*EEPROM_PAGE_SIZE+j),HEX); 
    };
    USB.println();
  };
}

// all EEPORM write checked to reduce wear 
void eeSave(uint8_t addr, uint8_t value) {
  if (value != EEPROM.read(addr))
    EEPROM.write(addr, value);
}