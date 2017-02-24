/*
 Print.cpp - Base class that provides print() and println()
 Copyright (c) 2008 David A. Mellis.  All right reserved.
 many modifications, by Paul Stoffregen <paul@pjrc.com>
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 
 Modified 23 November 2006 by David A. Mellis
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <avr/pgmspace.h>
#include "wiring.h"

#include "Print.h"

#if ARDUINO >= 100
#else
void Print::write(const char *str)
{
    write((const uint8_t *)str, strlen(str));
}
#endif


#if ARDUINO >= 100
size_t Print::write(const uint8_t *buffer, size_t size)
{
    size_t count = 0;
    while (size--) count += write(*buffer++);
    return count;
}
#else
void Print::write(const uint8_t *buffer, size_t size)
{
    while (size--) write(*buffer++);
}
#endif


#if ARDUINO >= 100
size_t Print::print(const String &s)
{
    uint8_t buffer[33];
    size_t count = 0;
    unsigned int index = 0;
    unsigned int len = s.length();
    while (len > 0) {
        s.getBytes(buffer, sizeof(buffer), index);
        unsigned int nbytes = len;
        if (nbytes > sizeof(buffer)-1) nbytes = sizeof(buffer)-1;
        index += nbytes;
        len -= nbytes;
        count += write(buffer, nbytes);
    }
    return count;
}
#else
void Print::print(const String &s)
{
    unsigned int len = s.length();
    for (unsigned int i=0; i < len; i++) {
        write(s[i]);
    }
}
#endif


#if ARDUINO >= 100
size_t Print::print(const __FlashStringHelper *ifsh)
{
    uint8_t buffer[32];
    size_t count = 0;
    const char PROGMEM *p = (const char PROGMEM *)ifsh;
    unsigned int len = strlen_P(p);
    while (len > 0) {
        unsigned int nbytes = len;
        if (nbytes > sizeof(buffer)) nbytes = sizeof(buffer);
        memcpy_P(buffer, p, nbytes);
        p += nbytes;
        len -= nbytes;
        count += write(buffer, nbytes);
    }
    return count;
}
#else
void Print::print(const __FlashStringHelper *ifsh)
{
    const char PROGMEM *p = (const char PROGMEM *)ifsh;
    while (1) {
        unsigned char c = pgm_read_byte(p++);
        if (c == 0) return;
        write(c);
    }
}
#endif


#if ARDUINO >= 100
size_t Print::print(long n)
{
    uint8_t sign=0;

    if (n < 0) {
        sign = '-';
        n = -n;
    }
    return printNumber(n, 10, sign);
}
#else
void Print::print(long n)
{
    uint8_t sign=0;

    if (n < 0) {
        sign = '-';
        n = -n;
    }
    printNumber(n, 10, sign);
}
#endif


#if ARDUINO >= 100
size_t Print::println(void)
{
    uint8_t buf[2]={'\r', '\n'};
    return write(buf, 2);
}
#else
void Print::println(void)
{
    uint8_t buf[2]={'\r', '\n'};
    write(buf, 2);
}
#endif


#if ARDUINO >= 100
size_t Print::printNumber(unsigned long n, uint8_t base, uint8_t sign)
#else
void Print::printNumber(unsigned long n, uint8_t base, uint8_t sign)
#endif
{
    uint8_t buf[8 * sizeof(long) + 1];
    uint8_t digit, i;

    // TODO: make these checks as inline, since base is
    // almost always a constant.  base = 0 (BYTE) should
    // inline as a call directly to write()
    if (base == 0) {
#if ARDUINO >= 100
        return write((uint8_t)n);
#else
        write((uint8_t)n);
        return;
#endif
    } else if (base == 1) {
        base = 10;
    }

    if (n == 0) {
        buf[sizeof(buf) - 1] = '0';
        i = sizeof(buf) - 1;
    } else {
        i = sizeof(buf) - 1;
        while (1) {
            digit = n % base;
            buf[i] = ((digit < 10) ? '0' + digit : 'A' + digit - 10);
            n /= base;
            if (n == 0) break;
            i--;
        }
    }
    if (sign) {
        i--;
        buf[i] = '-';
    }
#if ARDUINO >= 100
    return write(buf + i, sizeof(buf) - i);
#else
    write(buf + i, sizeof(buf) - i);
#endif
}


#if ARDUINO >= 100
size_t Print::printFloat(double number, uint8_t digits) 
#else
void Print::printFloat(double number, uint8_t digits) 
#endif
{
    uint8_t sign=0;
#if ARDUINO >= 100
    size_t count=0;
#endif

    // Handle negative numbers
    if (number < 0.0) {
        sign = 1;
        number = -number;
    }

    // Round correctly so that print(1.999, 2) prints as "2.00"
    double rounding = 0.5;
    for (uint8_t i=0; i<digits; ++i) {
        rounding *= 0.1;
    }
    number += rounding;

    // Extract the integer part of the number and print it
    unsigned long int_part = (unsigned long)number;
    double remainder = number - (double)int_part;
#if ARDUINO >= 100
    count += printNumber(int_part, 10, sign);
#else
    printNumber(int_part, 10, sign);
#endif

    // Print the decimal point, but only if there are digits beyond
    if (digits > 0) {
        uint8_t n, buf[8], count=1;
        buf[0] = '.';

        // Extract digits from the remainder one at a time
        if (digits > sizeof(buf) - 1) digits = sizeof(buf) - 1;

        while (digits-- > 0) {
            remainder *= 10.0;
            n = (uint8_t)(remainder);
            buf[count++] = '0' + n;
            remainder -= n; 
        }
#if ARDUINO >= 100
        count += write(buf, count);
#else
        write(buf, count);
#endif
    }
#if ARDUINO >= 100
    return count;
#endif
}

// CanyonCasa: simple printf-like function...
#if ARDUINO >= 100
#define TMP_BUF_LEN 10  // only needs to hold 9 char field expansions
int Print::spf(const __FlashStringHelper *P_fmt, ... ) {
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

  const char PROGMEM *fmt = (const char PROGMEM *)P_fmt;  // recast
  va_list args;                         // declare args as va_list type
  va_start (args, P_fmt);               // point args to 1st after fmt
  uint8_t plen = 0;                     // printed length

  // parse the fmt string character by character to tmp array.
  while (unsigned char ch = pgm_read_byte(fmt++)) { // checks for null!
    char tmp[TMP_BUF_LEN];     // init temp buffer and other variables
    memset(tmp,0,TMP_BUF_LEN); // fill tmp with nulls, i.e. tmp = ''
    int width=0; uint8_t cnt=0; char pad=' '; char *sptr; uint8_t base=10;
    if (bool meta = (ch=='%')) { // possible meta marker
      while (meta && (ch=pgm_read_byte(fmt++))) { // scan metadata
        switch (ch) { // test the next character
          case '%':
            tmp[0] = ch; // means %%, pass % to output, end scanning
            meta = false;
            break;
          case '#': // count specifier, read arg to get value
            cnt = (uint8_t) va_arg(args,int);
            break;
          case '0':
            pad = '0';
            break;
          case '1' ... '9':
            width = ch&0xF;
            break;
          default:
            // must be a specifier...
            switch (ch|0x20) { // force lower case check
              case 's': // string, printed directly, padding placed in tmp
                sptr = va_arg(args, char *);  // get string pointer
                if ((cnt==0)||(cnt>strlen(sptr))) // bound cnt 
                  cnt = strlen(sptr);
                plen += cnt;         // add to print length
                write((const uint8_t *)sptr, cnt);   // print [sub]string
                // prep tmp with any padding, otherwise tmp = ''
                width = width - cnt;
                if (width > 0)
                  memset(tmp,' ',width);
                cnt = 0;
                break;
              case 'c': // copy var (promoted to int) to tmp[0]
                tmp[0] = (char) va_arg(args,int);
                break;
              case 'x': // hex int var
                base = 16;
              default:  // assume int
                cnt = 0; // remove any repeat action
                itoa(va_arg(args,int),tmp,base);
                if (base == 16) // always assume uppercase hex
                  strupr(tmp);
                width = width - strlen(tmp);  // set width to padding size
                if (width > 0) { // pad
                  memmove(tmp+width,tmp,strlen(tmp)+1); // move tmp to right
                  memset(tmp,pad,width); // prefix padding
                };
            };
            meta = false;
        };
      };
    } else {
      tmp[0] = ch; // passthrough ch
    };
    // passthrough ch, escape character, or meta value in tmp
    do {
      plen += strlen(tmp);
      write(tmp);
    } while (cnt-- > 1); // always print once, repeat if necessary
  };

  va_end (args); // free arg list
  return plen;   // printed length
}
#endif
// ...CanyonCasa
