#pragma once

#include "system.h"
#include "AudioSystem.h"
#include "ReferenceManager.h"
#include "SoundBuffer.h"

struct stb_vorbis;

class CMusic : public CReferenceCounter, CAllocable<CMusic>, public CSoundBuffer
{
public:
    CMusic(LPSTR path);
    VOID Release();
    VOID Update();
    VOID Stop() override;
protected:
    VOID ResetPosition();
    VOID PullSamples(ULONG offset, ULONG reqBytes, BOOL initCursors = FALSE);

    stb_vorbis* mDecoder;
    WAVEFORMATEX mWaveInfo;
    HANDLE mEvents[2];
    ULONG mOffset;
    UINT mId;
};
