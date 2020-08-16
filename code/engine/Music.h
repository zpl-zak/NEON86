#pragma once

#include "system.h"
#include "AudioSystem.h"
#include "ReferenceManager.h"
#include "SoundBase.h"

struct stb_vorbis;

class CMusic : public CReferenceCounter, CAllocable<CMusic>, public CSoundBase
{
public:
    CMusic(LPSTR path);
    VOID Release();
    VOID Update();
    VOID Stop() override;
    virtual DWORD GetCurrentPosition() override;
    virtual VOID SetCurrentPosition(DWORD cursor) override;
    virtual DWORD GetTotalSize() override;
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
