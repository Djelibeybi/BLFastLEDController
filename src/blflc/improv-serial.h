#ifndef _BLFLC_IMPROV_SERIAL_H
#define _BLFLC_IMPROV_SERIAL_H

#include <Arduino.h>

// Initialize Improv Serial - call this in setup() when no credentials exist
void setupImprovSerial();

// Process Improv Serial - call this in loop()
void loopImprovSerial();

#endif // _BLFLC_IMPROV_SERIAL_H
