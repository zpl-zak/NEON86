#pragma once

#include "system.h"

#include <mmsystem.h>
#include <dsound.h>

#include "StdAfx.h"

#pragma comment (lib, "dsound.lib")
#pragma comment (lib, "winmm.lib")

#define AUDIO CEngine::the()->GetAudioSystem()

class ENGINE_API CAudioSystem
{
public:
    CAudioSystem();
    HRESULT CreateDevice(HWND);
    VOID Release();

    inline IDirectSound8* GetDevice() { return mDirectSound; }
    inline IDirectSound3DListener8* GetListener() { return mListener; }

private:
    BOOL mIsInitialized;
    IDirectSound8* mDirectSound;
    IDirectSoundBuffer* mPrimaryBuffer;
    IDirectSound3DListener8* mListener;
};
