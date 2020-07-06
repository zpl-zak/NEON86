#pragma once

#include "system.h"
#include "AudioSystem.h"
#include "ReferenceManager.h"

class CSound: public CReferenceCounter, CAllocable<CSound>
{
public:
    CSound(LPSTR wavPath);
    VOID Release();

    VOID Play();
    VOID Pause();
    VOID Stop();
    VOID Update();
    VOID SetVolume(LONG vol);
    VOID SetPan(LONG pan);
    LONG GetVolume();
    LONG GetPan();
    BOOL IsPlaying();
    DWORD GetCurrentPosition();
    VOID SetCurrentPosition(DWORD cursor);
    VOID SetLoop(BOOL state);
    BOOL IsLooping();
private:
    IDirectSoundBuffer8* mBuffer;
    BOOL mIsLooping;
};
