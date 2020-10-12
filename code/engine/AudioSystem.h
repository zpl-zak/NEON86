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
    ~CAudioSystem();
    auto CreateDevice(HWND) -> HRESULT;
    auto Release() -> void;
    auto Update() -> void;

    auto RegisterTrack(CMusic* track) -> unsigned int;
    auto UnregisterTrack(unsigned int idx) -> void;

    auto GetDevice() const -> IDirectSound8* { return mDirectSound; }
    auto GetListener() const -> IDirectSound3DListener8* { return mListener; }

    // TODO: Expose audio listener for 3D audio support

private:
    bool mIsInitialized;
    IDirectSound8* mDirectSound;
    IDirectSoundBuffer* mPrimaryBuffer;
    IDirectSound3DListener8* mListener;
    CArray<CMusic*> mTracks;
    unsigned int mTrackId;
};
