#include "StdAfx.h"

#include "Music.h"
#include "SoundLoader.h"

#include "engine.h"
#include "VM.h"
#include "AudioSystem.h"

#include <string>

CMusic::CMusic(LPSTR path) : CAllocable()
{
    mBuffer = nullptr;
    mDecoder = nullptr;
    mOffset = 0;

    mEvents[0] = CreateEventA(nullptr, FALSE, FALSE, nullptr);
    mEvents[1] = CreateEventA(nullptr, FALSE, FALSE, nullptr);

    CSoundLoader::OpenOGG(&mDecoder, path, &mBuffer, mEvents, &mWaveInfo, &mDataSize);
    PullSamples(0, mWaveInfo.nAvgBytesPerSec, true);
    mId = AUDIO->RegisterTrack(this);
}

void CMusic::Release()
{
    if (DelRef())
    {
        SAFE_RELEASE(mBuffer);
        CloseHandle(mEvents[0]);
        CloseHandle(mEvents[1]);
        CSoundLoader::CloseOGG(mDecoder);
        AUDIO->UnregisterTrack(mId);
        delete this;
    }
}

void CMusic::Update()
{
    HRESULT hr = WaitForMultipleObjects(2, mEvents, FALSE, 0);

    if (hr == WAIT_OBJECT_0)
    {
        PullSamples(mWaveInfo.nAvgBytesPerSec, mWaveInfo.nAvgBytesPerSec);
    }
    else if (hr == (WAIT_OBJECT_0 + 1))
    {
        PullSamples(0, mWaveInfo.nAvgBytesPerSec);
    }
}

void CMusic::Stop()
{
    Pause();
    ResetPosition();
}

DWORD CMusic::GetCurrentPosition()
{
    return CSoundLoader::TellOGG(mDecoder);
}

void CMusic::SetCurrentPosition(DWORD cursor)
{
    PushLog("Can't seek in OGG yet!");
}

DWORD CMusic::GetTotalSize()
{
    return mDataSize;
}

void CMusic::ResetPosition()
{
    mBuffer->SetCurrentPosition(0);
    CSoundLoader::ResetBuffer(mDecoder);
    ResetEvent(mEvents[0]);
    ResetEvent(mEvents[1]);
    LPVOID audio1 = nullptr;
    DWORD audioLen1 = 0;

    HRESULT hr = mBuffer->Lock(0, 0, &audio1, &audioLen1, nullptr, nullptr, DSBLOCK_ENTIREBUFFER);
    if (FAILED(hr))
    {
        VM->PostError("Failed to lock music buffer!");
        return;
    }
    memset(audio1, 0, audioLen1);
    mBuffer->Unlock(audio1, audioLen1, nullptr, NULL);
}

void CMusic::PullSamples(ULONG offset, ULONG reqBytes, bool initCursors)
{
    LPVOID audio1 = nullptr, audio2 = nullptr;
    DWORD audioLen1 = 0, audioLen2 = 0;
    HRESULT result;

    UCHAR* data;
    unsigned int n = static_cast<unsigned int>(CSoundLoader::DecodeOGG(mDecoder, reqBytes, (short**)&data));

    if (n == 0)
    {
        Stop();
        return;
    }

    result = mBuffer->Lock(offset, reqBytes, &audio1, &audioLen1, &audio2, &audioLen2, 0);
    if (FAILED(result))
    {
        VM->PostError("Failed to lock music buffer!");
        delete[] data;
        return;
    }

    if (initCursors)
    {
        if (audio1) memset(audio1, 0, audioLen1);
        if (audio2) memset(audio2, 0, audioLen2);
    }

    if (n < reqBytes / 2)
    {
        memset(data + n, 0, reqBytes / 2 - n);
    }

    if (audio2 == nullptr)
    {
        memcpy(audio1, data, audioLen1);
    }
    else
    {
        memcpy(audio1, data, audioLen1);
        memcpy(audio2, data + audioLen1, audioLen2);
    }

    delete[] data;

    result = mBuffer->Unlock(audio1, audioLen1, audio2, audioLen2);
    if (FAILED(result))
    {
        VM->PostError("Failed to unlock music buffer!");
    }
}
