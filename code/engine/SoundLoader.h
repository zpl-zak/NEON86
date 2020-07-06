#pragma once

#include "system.h"

struct IDirectSoundBuffer8;

class CSoundLoader
{
public:
    static VOID LoadWAV(LPSTR wavPath, IDirectSoundBuffer8** sndBuffer);
};