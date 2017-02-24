Test of Adding printf function to Tensy 1.0 core...
===================================================

## Summary

Teensy core 1.0 doesn't include a printf function. Simply copied the printf headers from the Teensy 3 library print.h file and the corresponding functions from the print.cpp file.

This code simply tests various print formats supported by the change.

Basic usage:

printf(F("use a progmem string for the format spec"), arg1, arg2, ...);

NOTES:
1. Does not appear to support F("") usage other than first argument.
2. Does not print floating point.
