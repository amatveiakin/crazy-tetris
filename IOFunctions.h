#ifndef CRAZYTETRIS_IOFUNCTIONS_H
#define CRAZYTETRIS_IOFUNCTIONS_H

#include "Declarations.h"

void registerKeyHandler(RealKey key, int reactivateTime,
                        void* keyHandler(ControlKey keyType, int iPlayer));
void unregisterKeyHandler(RealKey key);

#endif
