#ifndef CRAZYTETRIS_IOFUNCTIONS_H
#define CRAZYTETRIS_IOFUNCTIONS_H

#include "Declarations.h"

typedef int RealKey;
const int FIRST_REAL_KEY = 0;
const int LAST_REAL_KEY = 1023;   // TODO: find real value
const int N_REAL_KEYS = LAST_REAL_KEY - FIRST_REAL_KEY + 1;

// void registerKeyHandler(RealKey key, Time reactivateTime, void* keyHandler(RealKey key));
// void unregisterKeyHandler(RealKey key);
// void unregisterAllKeyHandlers();

bool keyPressed(RealKey key);

#endif
