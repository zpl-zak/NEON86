#include "StdAfx.h"

#include "Sound.h"
#include "SoundLoader.h"
#include "engine.h"
#include "VM.h"

CSound::CSound(LPSTR wavPath): CAllocable()
{
    mBuffer = NULL;
    mIsLooping = FALSE;
    mData = NULL;
    mDataSize = 0;

    CString path(wavPath);

    if (path.Find(".ogg")) {
        CSoundLoader::LoadOGG(wavPath, &mBuffer, &mData, &mDataSize, &mWaveInfo);
    }
    if (path.Find(".wav")) {
        CSoundLoader::LoadWAV(wavPath, &mBuffer, &mData, &mDataSize, &mWaveInfo);
    }
    else {
        VM->PostError(CString::Format("Unsupported sound file: %s !", path).Str());
    }
}

VOID CSound::Release()
{
    if (DelRef())
    {
        SAFE_RELEASE(mBuffer);
        SAFE_DELETE_ARRAY(mData);
        delete this;
    }
}

VOID CSound::Play()
{
    mBuffer->Play(0, 0, mIsLooping ? DSBPLAY_LOOPING : 0);
}

VOID CSound::Pause()
{
    mBuffer->Stop();
}

VOID CSound::Stop()
{
    Pause();
    SetCurrentPosition(0);
}

VOID CSound::SetFrequency(DWORD freq)
{
    if (freq == 0)
    {
        freq = DSBFREQUENCY_ORIGINAL;
    }

    mBuffer->SetFrequency(freq);
}

DWORD CSound::GetFrequency()
{
    DWORD freq = 0;
    mBuffer->GetFrequency(&freq);
    return freq;
}

VOID CSound::SetLoop(BOOL state)
{
    mIsLooping = state;

    if (IsPlaying())
    {
        Pause();
        Play();
    }
}

BOOL CSound::IsLooping()
{
    return mIsLooping;
}

UCHAR* CSound::GetData(ULONG* sizeOut)
{
    *sizeOut = mDataSize;
    return mData;
}
