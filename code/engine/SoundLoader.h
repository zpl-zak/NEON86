#pragma once

#include "system.h"

struct IDirectSoundBuffer8;
struct stb_vorbis;

class CSoundLoader
{
public:
    static VOID LoadOGG(LPSTR path, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize, LPVOID waveInfo);
    static VOID LoadWAV(LPSTR path, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize, LPVOID waveInfo);
    static VOID LoadWAV3D(LPSTR path, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize, LPVOID waveInfo);

    // Music
    static VOID OpenOGG(stb_vorbis** outDecoder, LPSTR path, IDirectSoundBuffer8** sndBuffer, HANDLE* events, LPVOID waveInfo, DWORD *dataSize);
    static VOID ResetBuffer(stb_vorbis* decoder);
    static ULONG DecodeOGG(stb_vorbis* decoder, ULONG reqBytes, short** data);
    static DWORD TellOGG(stb_vorbis* decoder);
    static VOID CloseOGG(stb_vorbis* decoder);
};
