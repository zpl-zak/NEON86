#pragma once

#include "system.h"
#include "AudioSystem.h"
#include "ReferenceManager.h"

struct stb_vorbis;

class CMusic : public CReferenceCounter, CAllocable<CMusic>
{
public:
    CMusic(LPSTR path);
    VOID Release();
    VOID Update();
    VOID Play();
    VOID Pause();
    VOID Stop();
    VOID SetVolume(LONG vol);
    VOID SetPan(LONG pan);
    LONG GetVolume();
    LONG GetPan();
    BOOL IsPlaying();

protected:
    VOID ResetPosition();
    VOID PullSamples(ULONG offset, ULONG reqBytes, BOOL initCursors = FALSE);

    IDirectSoundBuffer8* mBuffer;
    stb_vorbis* mDecoder;
    WAVEFORMATEX mWaveInfo;
    HANDLE mEvents[2];
    ULONG mOffset;
    UINT mId;
};
