#pragma once

#include "system.h"

struct IDirectSoundBuffer8;
struct stb_vorbis;

class CSoundLoader
{
public:
    static void LoadOGG(LPSTR path, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize, LPVOID waveInfo);
    static void LoadWAV(LPSTR path, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize, LPVOID waveInfo);
    static void LoadWAV3D(LPSTR path, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize,
                          LPVOID waveInfo);

    // Music
    static void OpenOGG(stb_vorbis** outDecoder, LPSTR path, IDirectSoundBuffer8** sndBuffer, HANDLE* events,
                        LPVOID waveInfo, DWORD* dataSize);
    static void ResetBuffer(stb_vorbis* decoder);
    static auto DecodeOGG(stb_vorbis* decoder, ULONG reqBytes, short** data) -> ULONG;
    static auto TellOGG(stb_vorbis* decoder) -> DWORD;
    static void CloseOGG(stb_vorbis* decoder);
};
