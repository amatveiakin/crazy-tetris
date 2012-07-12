#ifndef CRAZYTETRIS_IOFUNCTIONS_H
#define CRAZYTETRIS_IOFUNCTIONS_H

#include "Declarations.h"

typedef int RealKey;
const int BEGIN_REAL_KEY = 0;
const int END_REAL_KEY = 256; // TODO: find real value
const int N_REAL_KEYS = END_REAL_KEY - BEGIN_REAL_KEY;

// void registerKeyHandler(RealKey key, int reactivateTime, ControlKey controlKey, int iPlayer,
//                         void* keyHandler(ControlKey controlKey, int iPlayer));
void registerKeyHandler(RealKey key, int reactivateTime, void* keyHandler(RealKey key));
void unregisterKeyHandler(RealKey key);

#endif
