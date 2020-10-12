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
    VOID Release();
    VOID Update();
    VOID Stop() override;
    auto GetCurrentPosition() -> DWORD override;
    VOID SetCurrentPosition(DWORD cursor) override;
    auto GetTotalSize() -> DWORD override;
protected:
    VOID ResetPosition();
    VOID PullSamples(ULONG offset, ULONG reqBytes, BOOL initCursors = FALSE);

    stb_vorbis* mDecoder;
    WAVEFORMATEX mWaveInfo;
    HANDLE mEvents[2];
    ULONG mOffset;
    UINT mId;
    DWORD mDataSize;
};
