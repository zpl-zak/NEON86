#pragma once

#include "system.h"
#include "AudioSystem.h"

class CSoundBase
{
public:
    void SetVolume(LONG vol);
    auto GetVolume() -> LONG;
    virtual void Play();
    virtual void Pause();
    virtual void Stop();
    void SetPan(LONG pan);
    auto GetPan() -> LONG;
    virtual auto GetCurrentPosition() -> DWORD;
    virtual void SetCurrentPosition(DWORD cursor);
    virtual auto IsPlaying() -> bool;
    virtual auto GetTotalSize() -> DWORD;

protected:
    IDirectSoundBuffer8* mBuffer;
    WAVEFORMATEX mWaveInfo;
};
