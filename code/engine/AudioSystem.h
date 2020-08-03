#pragma once

#include "system.h"

#include <mmsystem.h>
#include <dsound.h>

#include "StdAfx.h"

#pragma comment (lib, "dsound.lib")
#pragma comment (lib, "winmm.lib")

#define AUDIO CEngine::the()->GetAudioSystem()

class CMusic;

class ENGINE_API CAudioSystem
{
public:
    CAudioSystem();
    HRESULT CreateDevice(HWND);
    VOID Release();
    VOID Update();

    UINT RegisterTrack(CMusic* track);
    VOID UnregisterTrack(UINT idx);

    inline IDirectSound8* GetDevice() { return mDirectSound; }
    inline IDirectSound3DListener8* GetListener() { return mListener; }

private:
    BOOL mIsInitialized;
    IDirectSound8* mDirectSound;
    IDirectSoundBuffer* mPrimaryBuffer;
    IDirectSound3DListener8* mListener;
    CArray<CMusic*> mTracks;
    UINT mTrackId;
};
