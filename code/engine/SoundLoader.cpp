#include "StdAfx.h"
#include "SoundLoader.h"
#include <mmsystem.h>
#include <dsound.h>

#include "NeonEngine.h"

#pragma comment(lib, "dxguid.lib")

struct WAVEHEADER
{
    char chunkId[4];
    unsigned long chunkSize;
    char format[4];
    char subChunkId[4];
    unsigned long subChunkSize;
    unsigned short audioFormat;
    unsigned short numChannels;
    unsigned long sampleRate;
    unsigned long bytesPerSecond;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    char dataChunkId[4];
    unsigned long dataSize;
};

VOID CSoundLoader::LoadWAV(LPSTR wavPath, IDirectSoundBuffer8** sndBuffer)
{
    FILE* fp;
    unsigned int count;
    WAVEHEADER waveFileHeader;
    WAVEFORMATEX waveFormat;
    DSBUFFERDESC bufferDesc;
    HRESULT result;
    IDirectSoundBuffer* tempBuffer;
    unsigned char* waveData;
    unsigned char* bufferPtr;
    unsigned long bufferSize;

    fp = FILESYSTEM->OpenResource(RESOURCEKIND_USER, wavPath);

    if (!fp)
    {
        VM->PostError("Sound file: " + std::string(wavPath) + " does not exist!");
        return;
    }

    count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, fp);
    if (count != 1)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid!");
        return;
    }

    if (strncmp("RIFF", waveFileHeader.chunkId, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (wrong ID)!");
        return;
    }

    if (strncmp("WAVE", waveFileHeader.format, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (wrong format)!");
        return;
    }

    if (strncmp("fmt ", waveFileHeader.subChunkId, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (wrong subID)!");
        return;
    }

    if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (wrong audio format)!");
        return;
    }

    if (waveFileHeader.numChannels != 2)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (needs 2 audio channels)!");
        return;
    }

    if (waveFileHeader.sampleRate != 44100)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (needs to be 44.1KHz)!");
        return;
    }

    if (waveFileHeader.bitsPerSample != 16)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (needs to be 16-bit stereo)!");
        return;
    }

    if (strncmp("data", waveFileHeader.dataChunkId, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (missing DATA marker)!");
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
    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
    bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = &waveFormat;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    result = AUDIO->GetDevice()->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
    if (FAILED(result))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Failed to create temporary sound buffer for file: " + std::string(wavPath));
        return;
    }

    result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&*sndBuffer);
    if (FAILED(result))
    {
        tempBuffer->Release();
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Failed to create sound buffer for file: " + std::string(wavPath));
        return;
    }

    tempBuffer->Release();

    IDirectSoundBuffer8* snd = *sndBuffer;

    fseek(fp, sizeof(WAVEHEADER), SEEK_SET);
    waveData = new unsigned char[waveFileHeader.dataSize];
    if (!waveData)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Failed to allocate data for sound buffer for file: " + std::string(wavPath));
        return;
    }

    count = fread(waveData, 1, waveFileHeader.dataSize, fp);
    if (count != waveFileHeader.dataSize)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Failed to read data for sound buffer for file: " + std::string(wavPath));
        return;
    }

    FILESYSTEM->CloseResource(fp);

    result = snd->Lock(0, waveFileHeader.dataSize, (LPVOID*)&bufferPtr, (LPDWORD)&bufferSize, NULL, 0, 0);
    if (FAILED(result))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Failed to lock sound buffer for file: " + std::string(wavPath));
        return;
    }

    memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

    result = snd->Unlock((LPVOID)bufferPtr, bufferSize, NULL, 0);
    if (FAILED(result))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Failed to unlock sound buffer for file: " + std::string(wavPath));
        return;
    }

    delete[] waveData;
}

VOID CSoundLoader::LoadWAV3D(LPSTR wavPath, IDirectSoundBuffer8** sndBuffer)
{
    FILE* fp;
    unsigned int count;
    WAVEHEADER waveFileHeader;
    WAVEFORMATEX waveFormat;
    DSBUFFERDESC bufferDesc;
    HRESULT result;
    IDirectSoundBuffer* tempBuffer;
    unsigned char* waveData;
    unsigned char* bufferPtr;
    unsigned long bufferSize;

    fp = FILESYSTEM->OpenResource(RESOURCEKIND_USER, wavPath);

    if (!fp)
    {
        VM->PostError("Sound file: " + std::string(wavPath) + " does not exist!");
        return;
    }

    count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, fp);
    if (count != 1)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid!");
        return;
    }

    if (strncmp("RIFF", waveFileHeader.chunkId, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (wrong ID)!");
        return;
    }

    if (strncmp("WAVE", waveFileHeader.format, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (wrong format)!");
        return;
    }

    if (strncmp("fmt ", waveFileHeader.subChunkId, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (wrong subID)!");
        return;
    }

    if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (wrong audio format)!");
        return;
    }

    if (waveFileHeader.numChannels != 1)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (needs 1 audio channel)!");
        return;
    }

    if (waveFileHeader.sampleRate != 44100)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (needs to be 44.1KHz)!");
        return;
    }

    if (waveFileHeader.bitsPerSample != 16)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (needs to be 16-bit stereo)!");
        return;
    }

    if (strncmp("data", waveFileHeader.dataChunkId, 4))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Sound file: " + std::string(wavPath) + " is invalid (missing DATA marker)!");
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
        VM->PostError("Failed to create temporary sound buffer for file: " + std::string(wavPath));
        return;
    }

    result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&*sndBuffer);
    if (FAILED(result))
    {
        tempBuffer->Release();
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Failed to create sound buffer for file: " + std::string(wavPath));
        return;
    }

    tempBuffer->Release();

    IDirectSoundBuffer8* snd = *sndBuffer;

    fseek(fp, sizeof(WAVEHEADER), SEEK_SET);
    waveData = new unsigned char[waveFileHeader.dataSize];
    if (!waveData)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Failed to allocate data for sound buffer for file: " + std::string(wavPath));
        return;
    }

    count = fread(waveData, 1, waveFileHeader.dataSize, fp);
    if (count != waveFileHeader.dataSize)
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Failed to read data for sound buffer for file: " + std::string(wavPath));
        return;
    }

    FILESYSTEM->CloseResource(fp);

    result = snd->Lock(0, waveFileHeader.dataSize, (LPVOID*)&bufferPtr, (LPDWORD)&bufferSize, NULL, 0, 0);
    if (FAILED(result))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Failed to lock sound buffer for file: " + std::string(wavPath));
        return;
    }

    memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

    result = snd->Unlock((LPVOID)bufferPtr, bufferSize, NULL, 0);
    if (FAILED(result))
    {
        FILESYSTEM->CloseResource(fp);
        VM->PostError("Failed to unlock sound buffer for file: " + std::string(wavPath));
        return;
    }

    delete[] waveData;
}
