#pragma once

#include "system.h"

struct IDirectSoundBuffer8;

class CSoundLoader
{
public:
    static VOID LoadOGG(LPSTR path, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize);
    static VOID LoadWAV(LPSTR path, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize);
    static VOID LoadWAV3D(LPSTR path, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize);
};
