#pragma once

#include "system.h"
#include "AudioSystem.h"
#include "ReferenceManager.h"
#include "SoundBuffer.h"

class CSound: public CReferenceCounter, CAllocable<CSound>, public CSoundBuffer
{
public:
    CSound(LPSTR wavPath);
    VOID Release();

    VOID Play() override;
    VOID Pause() override;
    VOID Stop() override;
    VOID SetFrequency(DWORD freq);
    DWORD GetFrequency();
    DWORD GetCurrentPosition();
    VOID SetCurrentPosition(DWORD cursor);
    VOID SetLoop(BOOL state);
    BOOL IsLooping();

    UCHAR* GetData(ULONG* sizeOut);
protected:
    BOOL mIsLooping;
    UCHAR* mData;
    ULONG mDataSize;
};
