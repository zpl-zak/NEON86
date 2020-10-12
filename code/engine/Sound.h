#pragma once

#include "system.h"
#include "AudioSystem.h"
#include "ReferenceManager.h"
#include "SoundBase.h"

class CSound : public CReferenceCounter, CAllocable<CSound>, public CSoundBase
{
public:
    CSound(LPSTR wavPath);
    VOID Release();

    VOID Play() override;
    VOID Pause() override;
    VOID Stop() override;
    VOID SetFrequency(DWORD freq);
    auto GetFrequency() -> DWORD;
    VOID SetLoop(BOOL state);
    auto IsLooping() -> BOOL;

    auto GetData(ULONG* sizeOut) -> UCHAR*;
protected:
    BOOL mIsLooping;
    UCHAR* mData;
    ULONG mDataSize;
};
