#pragma once

#include "system.h"

struct IDirectSoundBuffer8;

class CSoundLoader
{
public:
    static VOID LoadWAV(LPSTR wavPath, IDirectSoundBuffer8** sndBuffer);
    static VOID LoadWAV3D(LPSTR wavPath, IDirectSoundBuffer8** sndBuffer);
};