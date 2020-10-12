#pragma once

#include "AudioSystem.h"
#include "ReferenceManager.h"
#include "SoundBase.h"
#include "system.h"

struct stb_vorbis;

class CMusic : public CReferenceCounter, CAllocable<CMusic>, public CSoundBase
{
public:
    CMusic(LPSTR path);
    void Release();
    void Update();
    void Stop() override;
    auto GetCurrentPosition() -> DWORD override;
    void SetCurrentPosition(DWORD cursor) override;
    auto GetTotalSize() -> DWORD override;
protected:
    void ResetPosition();
    void PullSamples(ULONG offset, ULONG reqBytes, bool initCursors = FALSE);

    stb_vorbis* mDecoder;
    WAVEFORMATEX mWaveInfo;
    HANDLE mEvents[2];
    ULONG mOffset;
    unsigned int mId;
    DWORD mDataSize;
};
