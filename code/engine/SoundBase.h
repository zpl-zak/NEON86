#pragma once

#include "system.h"
#include "AudioSystem.h"

class CSoundBase
{
public:
    VOID SetVolume(LONG vol);
    auto GetVolume() -> LONG;
    virtual VOID Play();
    virtual VOID Pause();
    virtual VOID Stop();
    VOID SetPan(LONG pan);
    auto GetPan() -> LONG;
    virtual auto GetCurrentPosition() -> DWORD;
    virtual VOID SetCurrentPosition(DWORD cursor);
    virtual auto IsPlaying() -> BOOL;
    virtual auto GetTotalSize() -> DWORD;

protected:
    IDirectSoundBuffer8* mBuffer;
    WAVEFORMATEX mWaveInfo;
};
