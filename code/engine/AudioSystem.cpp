
#include "StdAfx.h"
#include "AudioSystem.h"
#include "Music.h"

#include <mmsystem.h>
#include <dsound.h>

CAudioSystem::CAudioSystem()
{
    mIsInitialized = FALSE;
    mDirectSound = NULL;
    mPrimaryBuffer = NULL;
    mListener = NULL;
    mTrackId = 0;
}

VOID CAudioSystem::Update()
{
    for (auto track : mTracks)
        track->Update();
}

HRESULT CAudioSystem::CreateDevice(HWND window)
{
    HRESULT result;
    DSBUFFERDESC bufferDesc;
    WAVEFORMATEX waveFormat;

    result = DirectSoundCreate8(NULL, &mDirectSound, NULL);
    if (FAILED(result))
    {
        Release();
        return result;
    }

    result = mDirectSound->SetCooperativeLevel(window, DSSCL_PRIORITY);
    if (FAILED(result))
    {
        Release();
        return result;
    }

    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
    bufferDesc.dwBufferBytes = 0;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = NULL;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    result = mDirectSound->CreateSoundBuffer(&bufferDesc, &mPrimaryBuffer, NULL);
    if (FAILED(result))
    {
        Release();
        return result;
    }

    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 2;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;

    result = mPrimaryBuffer->SetFormat(&waveFormat);
    if (FAILED(result))
    {
        Release();
        return result;
    }

    result = mPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&mListener);
    if (FAILED(result))
    {
        Release();
        return result;
    }

    mIsInitialized = TRUE;
    return ERROR_SUCCESS;
}

VOID CAudioSystem::Release()
{
    SAFE_RELEASE(mPrimaryBuffer);
    SAFE_RELEASE(mDirectSound);
    mIsInitialized = FALSE;
}

UINT CAudioSystem::RegisterTrack(CMusic* track)
{
    mTracks.Push(track);
    return mTrackId++;
}

VOID CAudioSystem::UnregisterTrack(UINT idx)
{
    mTracks.RemoveByIndex(idx);
}
