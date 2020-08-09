#include "StdAfx.h"
#include "SoundBuffer.h"

VOID CSoundBuffer::SetVolume(LONG vol)
{
    double attenuation = 1.0 / 1024.0 + vol / 100.0 * 1023.0 / 1024.0;
    double db = 10 * log10(attenuation) / log10(2);
    LONG realVol = (LONG)(db * 100);
    mBuffer->SetVolume(realVol);
}

LONG CSoundBuffer::GetVolume()
{
    LONG vol;
    mBuffer->GetVolume(&vol);
    float actVol = powf(10, (((float)vol) / 100.0f) * log10f(2) / 10.0f) * 100.0f;
    return (LONG)floor(actVol);
}

VOID CSoundBuffer::Play()
{
    mBuffer->Play(0, 0, DSBPLAY_LOOPING);
}

VOID CSoundBuffer::Pause()
{
    mBuffer->Stop();
}

VOID CSoundBuffer::Stop()
{
    Pause();
    mBuffer->SetCurrentPosition(0);
}

VOID CSoundBuffer::SetPan(LONG pan)
{
    LONG realPan = (LONG)ScaleBetween((FLOAT)pan, DSBPAN_LEFT, DSBPAN_RIGHT, -100, 100);
    mBuffer->SetPan(realPan);
}

LONG CSoundBuffer::GetPan()
{
    LONG pan;
    mBuffer->GetPan(&pan);
    return (LONG)ScaleBetween((FLOAT)pan, -100, 100, DSBPAN_LEFT, DSBPAN_RIGHT);
}


BOOL CSoundBuffer::IsPlaying()
{
    DWORD playStatus = 0x000000;
    mBuffer->GetStatus(&playStatus);
    return playStatus & DSBSTATUS_PLAYING;
}
