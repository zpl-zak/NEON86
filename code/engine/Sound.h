#pragma once

#include "system.h"
#include "AudioSystem.h"
#include "ReferenceManager.h"
#include "SoundBase.h"

class CSound : public CReferenceCounter, CAllocable<CSound>, public CSoundBase
{
public:
    CSound(LPSTR wavPath);
    void Release();

    void Play() override;
    void Pause() override;
    void Stop() override;
    void SetFrequency(DWORD freq);
    auto GetFrequency() -> DWORD;
    void SetLoop(bool state);
    auto IsLooping() -> bool;

    auto GetData(ULONG* sizeOut) -> UCHAR*;
protected:
    bool mIsLooping;
    UCHAR* mData;
    ULONG mDataSize;
};
