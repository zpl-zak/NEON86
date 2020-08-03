#pragma once

#include "system.h"

struct IDirectSoundBuffer8;
struct stb_vorbis;

class CSoundLoader
{
public:
    static VOID LoadOGG(LPSTR path, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize);
    static VOID LoadWAV(LPSTR path, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize);
    static VOID LoadWAV3D(LPSTR path, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize);

    // Music
    static VOID OpenOGG(stb_vorbis** outDecoder, LPSTR path, IDirectSoundBuffer8** sndBuffer, HANDLE* events, LPVOID waveInfo);
    static VOID ResetBuffer(stb_vorbis* decoder);
    static ULONG DecodeOGG(stb_vorbis* decoder, ULONG reqBytes, short** data);
    static VOID CloseOGG(stb_vorbis* decoder);
};
