#pragma once

#include "system.h"
#include "AudioSystem.h"

class CSoundBuffer
{
public:
    VOID SetVolume(LONG vol);
    LONG GetVolume();
    virtual VOID Play();
    virtual VOID Pause();
    virtual VOID Stop();
    VOID SetPan(LONG pan);
    LONG GetPan();
    virtual BOOL IsPlaying();

protected:
    IDirectSoundBuffer8* mBuffer;
};
