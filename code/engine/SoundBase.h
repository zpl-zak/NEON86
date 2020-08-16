#pragma once

#include "system.h"
#include "AudioSystem.h"

class CSoundBase
{
public:
    VOID SetVolume(LONG vol);
    LONG GetVolume();
    virtual VOID Play();
    virtual VOID Pause();
    virtual VOID Stop();
    VOID SetPan(LONG pan);
    LONG GetPan();
    virtual DWORD GetCurrentPosition();
    virtual VOID SetCurrentPosition(DWORD cursor);
    virtual BOOL IsPlaying();
    virtual DWORD GetTotalSize();

protected:
    IDirectSoundBuffer8* mBuffer;
    WAVEFORMATEX mWaveInfo;
};
