#include "StdAfx.h"

#include "Sound.h"
#include "SoundLoader.h"

CSound::CSound(LPSTR wavPath): CAllocable()
{
    mBuffer = NULL;
    mIsLooping = FALSE;
    CSoundLoader::LoadWAV(wavPath, &mBuffer);
}

VOID CSound::Release()
{
    if (DelRef())
    {
        SAFE_RELEASE(mBuffer);
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

VOID CSound::Update()
{
}

VOID CSound::SetVolume(LONG vol)
{
    mBuffer->SetVolume(vol);
}

VOID CSound::SetPan(LONG pan)
{
    mBuffer->SetPan(pan);
}

LONG CSound::GetVolume()
{
    LONG vol;
    mBuffer->GetVolume(&vol);
    return vol;
}

LONG CSound::GetPan()
{
    LONG pan;
    mBuffer->GetPan(&pan);
    return pan;
}

BOOL CSound::IsPlaying()
{
    DWORD playStatus = 0x000000;
    mBuffer->GetStatus(&playStatus);
    return playStatus & DSBSTATUS_PLAYING;
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
