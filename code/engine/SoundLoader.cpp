#include "StdAfx.h"
#include "SoundLoader.h"
#include <mmsystem.h>
#include <dsound.h>

#define STB_VORBIS_NO_PUSHDATA_API
#include "stb_vorbis.h"

#include "NeonEngine.h"

#pragma comment(lib, "dxguid.lib")

struct WAVEHEADER
{
    CHAR chunkId[4];
    ULONG chunkSize;
    CHAR format[4];
    CHAR subChunkId[4];
    ULONG subChunkSize;
    USHORT audioFormat;
    USHORT numChannels;
    ULONG sampleRate;
    ULONG bytesPerSecond;
    USHORT blockAlign;
    USHORT bitsPerSample;
    CHAR dataChunkId[4];
    ULONG dataSize;
};

VOID CSoundLoader::LoadWAV(LPSTR wavPath, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize)
{
    FILE* fp;
    size_t count;
    WAVEHEADER waveFileHeader;
    WAVEFORMATEX waveFormat;
    DSBUFFERDESC bufferDesc;
    HRESULT result;
    IDirectSoundBuffer* tempBuffer;
    UCHAR* waveData;
    UCHAR* bufferPtr;
    ULONG bufferSize;

    fp = FILESYSTEM->OpenResource(wavPath);

    if (!fp)
    {
        VM->PostError(CString::Format("Sound file: %s does not exist!", wavPath).Str());
        return;
    }

    count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, fp);
    if (count != 1)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid!", wavPath).Str());
        return;
    }

    if (strncmp("RIFF", waveFileHeader.chunkId, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (wrong ID)!", wavPath).Str());
        return;
    }

    if (strncmp("WAVE", waveFileHeader.format, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (wrong format)!", wavPath).Str());
        return;
    }

    if (strncmp("fmt ", waveFileHeader.subChunkId, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (wrong SubID)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (wrong audio format)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.numChannels != 2)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (needs 2 audio channels)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.sampleRate != 44100)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (needs to be 44.1KHz)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.bitsPerSample != 16)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (needs to be 16-bit stereo)!", wavPath).Str());
        return;
    }

    if (strncmp("data", waveFileHeader.dataChunkId, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (missing DATA marker)!", wavPath).Str());
        return;
    }

    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 2;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;

    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY;
    bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = &waveFormat;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    result = AUDIO->GetDevice()->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
    if (FAILED(result))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Failed to create temporary sound buffer for file: %s!", wavPath).Str());
        return;
    }

    result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&*sndBuffer);
    if (FAILED(result))
    {
        tempBuffer->Release();
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Failed to create sound buffer for file: %s!", wavPath).Str());
        return;
    }

    tempBuffer->Release();

    IDirectSoundBuffer8* snd = *sndBuffer;

    fseek(fp, sizeof(WAVEHEADER), SEEK_SET);
    waveData = new UCHAR[waveFileHeader.dataSize];
    if (!waveData)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Failed to allocate data for sound buffer for file: %s!", wavPath).Str());
        return;
    }

    *dataPtr = waveData;
    *dataSize = waveFileHeader.dataSize;

    count = fread(waveData, 1, waveFileHeader.dataSize, fp);
    if (count != waveFileHeader.dataSize)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Failed to read data for sound buffer for file: %s!", wavPath).Str());
        return;
    }

    FILESYSTEM->CloseResource(fp);

    result = snd->Lock(0, waveFileHeader.dataSize, (LPVOID*)&bufferPtr, (LPDWORD)&bufferSize, NULL, 0, 0);
    if (FAILED(result))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Failed to lock sound buffer for file: %s!", wavPath).Str());
        return;
    }

    memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

    result = snd->Unlock((LPVOID)bufferPtr, bufferSize, NULL, 0);
    if (FAILED(result))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Failed to unlock sound buffer for file: %s!", wavPath).Str());
        return;
    }
}

VOID CSoundLoader::LoadOGG(LPSTR oggPath, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize)
{
    size_t count;
    WAVEFORMATEX waveFormat;
    DSBUFFERDESC bufferDesc;
    HRESULT result;
    IDirectSoundBuffer* tempBuffer;
    UCHAR* bufferPtr;
    ULONG bufferSize;

    int channels;
    int sample_rate;
    short * output;
    count = stb_vorbis_decode_filename(FILESYSTEM->ResourcePath(oggPath), &channels, &sample_rate, &output);

    if (count == -1)
    {
        VM->PostError(CString::Format("Sound file: %s does not exist!", oggPath).Str());
        return;
    }

    if (channels != 2)
    {
        VM->PostError(CString::Format("Sound file: %s is invalid (needs 2 audio channels)!", oggPath).Str());
        return;
    }

    if (sample_rate != 44100)
    {
        VM->PostError(CString::Format("Sound file: %s is invalid (needs to be 44.1KHz)!", oggPath).Str());
        return;
    }

    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 2;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;

    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY;
    bufferDesc.dwBufferBytes = (DWORD)(count * channels * (sizeof(int16) / sizeof(uint8)));
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = &waveFormat;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    result = AUDIO->GetDevice()->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
    if (FAILED(result))
    {
        VM->PostError(CString::Format("Failed to create temporary sound buffer for file: %s!", oggPath).Str());
        return;
    }

    result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&*sndBuffer);
    if (FAILED(result))
    {
        tempBuffer->Release();
        VM->PostError(CString::Format("Failed to create sound buffer for file: %s!", oggPath).Str());
        return;
    }

    tempBuffer->Release();

    IDirectSoundBuffer8* snd = *sndBuffer;

    result = snd->Lock(0, bufferDesc.dwBufferBytes, (LPVOID*)&bufferPtr, (LPDWORD)&bufferSize, NULL, 0, 0);
    if (FAILED(result))
    {
        VM->PostError(CString::Format("Failed to lock sound buffer for file: %s!", oggPath).Str());
        return;
    }

    *dataPtr = (UCHAR *)output;
    *dataSize = (ULONG)count;
    memcpy(bufferPtr, (UCHAR *)output, bufferDesc.dwBufferBytes);

    result = snd->Unlock((LPVOID)bufferPtr, bufferSize, NULL, 0);
    if (FAILED(result))
    {
        VM->PostError(CString::Format("Failed to unlock sound buffer for file: %s!", oggPath).Str());
        return;
    }
}

VOID CSoundLoader::LoadWAV3D(LPSTR wavPath, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize)
{
    FILE* fp;
    size_t count;
    WAVEHEADER waveFileHeader;
    WAVEFORMATEX waveFormat;
    DSBUFFERDESC bufferDesc;
    HRESULT result;
    IDirectSoundBuffer* tempBuffer;
    UCHAR* waveData;
    UCHAR* bufferPtr;
    ULONG bufferSize;

    fp = FILESYSTEM->OpenResource(wavPath);

    if (!fp)
    {
        VM->PostError(CString::Format("Sound file: %s does not exist!", wavPath).Str());
        return;
    }

    count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, fp);
    if (count != 1)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid!", wavPath).Str());
        return;
    }

    if (strncmp("RIFF", waveFileHeader.chunkId, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (wrong ID)!", wavPath).Str());
        return;
    }

    if (strncmp("WAVE", waveFileHeader.format, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (wrong format)!", wavPath).Str());
        return;
    }

    if (strncmp("fmt ", waveFileHeader.subChunkId, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (wrong subID)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (wrong audio format)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.numChannels != 1)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (needs 1 audio channel)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.sampleRate != 44100)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (needs to be 44.1KHz)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.bitsPerSample != 16)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (needs to be 16-bit stereo)!", wavPath).Str());
        return;
    }

    if (strncmp("data", waveFileHeader.dataChunkId, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Sound file: %s is invalid (missing DATA marker)!", wavPath).Str());
        return;
    }

    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 1;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;

    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
    bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = &waveFormat;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    result = AUDIO->GetDevice()->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
    if (FAILED(result))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Failed to create temporary sound buffer for file: %s!", wavPath).Str());
        return;
    }

    result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&*sndBuffer);
    if (FAILED(result))
    {
        tempBuffer->Release();
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Failed to create sound buffer for file: %s!", wavPath).Str());
        return;
    }

    tempBuffer->Release();

    IDirectSoundBuffer8* snd = *sndBuffer;

    fseek(fp, sizeof(WAVEHEADER), SEEK_SET);
    waveData = new UCHAR[waveFileHeader.dataSize];
    if (!waveData)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Failed to allocate data for sound buffer for file: %s!", wavPath).Str());
        return;
    }

    *dataPtr = waveData;
    *dataSize = waveFileHeader.dataSize;

    count = fread(waveData, 1, waveFileHeader.dataSize, fp);
    if (count != waveFileHeader.dataSize)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Failed to read data for sound buffer for file: %s!", wavPath).Str());
        return;
    }

    FILESYSTEM->CloseResource(fp);

    result = snd->Lock(0, waveFileHeader.dataSize, (LPVOID*)&bufferPtr, (LPDWORD)&bufferSize, NULL, 0, 0);
    if (FAILED(result))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Failed to lock sound buffer for file: %s!", wavPath).Str());
        return;
    }

    memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

    result = snd->Unlock((LPVOID)bufferPtr, bufferSize, NULL, 0);
    if (FAILED(result))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError(CString::Format("Failed to unlock sound buffer for file: %s!", wavPath).Str());
        return;
    }
}
