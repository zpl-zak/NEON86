#include "StdAfx.h"

#include "Music.h"
#include "SoundLoader.h"

#include "engine.h"
#include "VM.h"
#include "AudioSystem.h"

#include <string>

CMusic::CMusic(LPSTR path) : CAllocable()
{
    mBuffer = NULL;
    mDecoder = NULL;
    mOffset = 0;

    mEvents[0] = CreateEventA(NULL, FALSE, FALSE, NULL);
    mEvents[1] = CreateEventA(NULL, FALSE, FALSE, NULL);

    CSoundLoader::OpenOGG(&mDecoder, path, &mBuffer, mEvents, &mWaveInfo, &mDataSize);
    PullSamples(0, mWaveInfo.nAvgBytesPerSec, true);
    mId = AUDIO->RegisterTrack(this);
}

VOID CMusic::Release()
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

VOID CMusic::Update()
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

VOID CMusic::Stop()
{
    Pause();
    ResetPosition();
}

DWORD CMusic::GetCurrentPosition()
{
    return CSoundLoader::TellOGG(mDecoder);
}

VOID CMusic::SetCurrentPosition(DWORD cursor)
{
    PushLog("Can't seek in OGG yet!");
}

DWORD CMusic::GetTotalSize()
{
    return mDataSize;
}

VOID CMusic::ResetPosition()
{
    mBuffer->SetCurrentPosition(0);
    CSoundLoader::ResetBuffer(mDecoder);
    ResetEvent(mEvents[0]);
    ResetEvent(mEvents[1]);
    LPVOID audio1 = NULL;
    DWORD audioLen1 = 0;

    HRESULT hr = mBuffer->Lock(0, 0, &audio1, &audioLen1, NULL, NULL, DSBLOCK_ENTIREBUFFER);
    if (FAILED(hr)) {
        VM->PostError("Failed to lock music buffer!");
        return;
    }
    memset(audio1, 0, audioLen1);
    mBuffer->Unlock(audio1, audioLen1, NULL, NULL);
}

VOID CMusic::PullSamples(ULONG offset, ULONG reqBytes, BOOL initCursors)
{
    LPVOID audio1 = NULL, audio2 = NULL;
    DWORD audioLen1 = 0, audioLen2 = 0;
    HRESULT result;

    UCHAR* data;
    UINT n = (UINT)CSoundLoader::DecodeOGG(mDecoder, reqBytes, (short**)&data);

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

    if (initCursors) {
        if (audio1) memset(audio1, 0, audioLen1);
        if (audio2) memset(audio2, 0, audioLen2);
    }

    if (n < reqBytes/2) {
        ::memset(data + n, 0, reqBytes/2 - n);
    }

    if (audio2 == NULL) {
        ::memcpy(audio1, data, audioLen1);
    }
    else {
        ::memcpy(audio1, data, audioLen1);
        ::memcpy(audio2, data+audioLen1, audioLen2);
    }
    
    delete[] data;

    result = mBuffer->Unlock(audio1, audioLen1, audio2, audioLen2);
    if (FAILED(result))
    {
        VM->PostError("Failed to unlock music buffer!");
        return;
    }
}
