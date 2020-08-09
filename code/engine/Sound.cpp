#include "StdAfx.h"

#include "Sound.h"
#include "SoundLoader.h"

#include <string>

CSound::CSound(LPSTR wavPath): CAllocable()
{
    mBuffer = NULL;
    mIsLooping = FALSE;
    mData = NULL;
    mDataSize = 0;

    std::string path(wavPath);

    if (path.rfind(".ogg") != std::string::npos) {
        CSoundLoader::LoadOGG(wavPath, &mBuffer, &mData, &mDataSize);
    }
    else {
        CSoundLoader::LoadWAV(wavPath, &mBuffer, &mData, &mDataSize);
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

DWORD CSound::GetCurrentPosition()
{
    DWORD playPos = 0x000000;
    mBuffer->GetCurrentPosition(&playPos, NULL);
    return playPos;
}

VOID CSound::SetCurrentPosition(DWORD cursor)
{
    mBuffer->SetCurrentPosition(cursor);
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
