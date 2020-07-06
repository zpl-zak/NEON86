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
    BOOL CreateDevice();
    VOID Release();

    VOID Update(FLOAT dt);
    inline IDirectSound8* GetDevice() { return mDirectSound; }

private:
    BOOL mIsInitialized;
    IDirectSound8* mDirectSound;
    IDirectSoundBuffer* mPrimaryBuffer;
};
