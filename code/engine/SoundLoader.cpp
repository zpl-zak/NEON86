#include "StdAfx.h"
#include "SoundLoader.h"
#include <mmsystem.h>
#include <dsound.h>

#define STB_VORBIS_NO_PUSHDATA_API
#define STB_VORBIS_HEADER_ONLY
#include "stb_vorbis.c"

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

void CSoundLoader::LoadWAV(LPSTR wavPath, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize,
                           LPVOID waveInfo)
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
    CFileSystem* fs = ENGINE->GetFileSystem();
    CVirtualMachine* vm = ENGINE->GetVM();

    fp = fs->OpenResource(wavPath);

    if (!fp)
    {
        vm->PostError(CString::Format("Sound file: %s does not exist!", wavPath).Str());
        return;
    }

    count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, fp);
    if (count != 1)
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid!", wavPath).Str());
        return;
    }

    if (strncmp("RIFF", waveFileHeader.chunkId, 4))
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (wrong ID)!", wavPath).Str());
        return;
    }

    if (strncmp("WAVE", waveFileHeader.format, 4))
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (wrong format)!", wavPath).Str());
        return;
    }

    if (strncmp("fmt ", waveFileHeader.subChunkId, 4))
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (wrong SubID)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (wrong audio format)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.numChannels != 2)
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (needs 2 audio channels)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.sampleRate != 44100)
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (needs to be 44.1KHz)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.bitsPerSample != 16)
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (needs to be 16-bit stereo)!", wavPath).Str());
        return;
    }

    if (strncmp("data", waveFileHeader.dataChunkId, 4))
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (missing DATA marker)!", wavPath).Str());
        return;
    }

    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 2;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;
    *static_cast<WAVEFORMATEX*>(waveInfo) = waveFormat;

    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY;
    bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = &waveFormat;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    result = AUDIO->GetDevice()->CreateSoundBuffer(&bufferDesc, &tempBuffer, nullptr);
    if (FAILED(result))
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Failed to create temporary sound buffer for file: %s!", wavPath).Str());
        return;
    }

    result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&*sndBuffer);
    if (FAILED(result))
    {
        tempBuffer->Release();
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Failed to create sound buffer for file: %s!", wavPath).Str());
        return;
    }

    tempBuffer->Release();

    IDirectSoundBuffer8* snd = *sndBuffer;

    fseek(fp, sizeof(WAVEHEADER), SEEK_SET);
    waveData = new UCHAR[waveFileHeader.dataSize];
    if (!waveData)
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Failed to allocate data for sound buffer for file: %s!", wavPath).Str());
        return;
    }

    *dataPtr = waveData;
    *dataSize = waveFileHeader.dataSize;

    count = fread(waveData, 1, waveFileHeader.dataSize, fp);
    if (count != waveFileHeader.dataSize)
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Failed to read data for sound buffer for file: %s!", wavPath).Str());
        return;
    }

    fs->CloseResource(fp);

    result = snd->Lock(0, waveFileHeader.dataSize, (LPVOID*)&bufferPtr, static_cast<LPDWORD>(&bufferSize), nullptr,
                       nullptr, 0);
    if (FAILED(result))
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Failed to lock sound buffer for file: %s!", wavPath).Str());
        return;
    }

    memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

    result = snd->Unlock(static_cast<LPVOID>(bufferPtr), bufferSize, nullptr, 0);
    if (FAILED(result))
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Failed to unlock sound buffer for file: %s!", wavPath).Str());
    }
}

void CSoundLoader::LoadOGG(LPSTR oggPath, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize,
                           LPVOID waveInfo)
{
    size_t count;
    WAVEFORMATEX waveFormat;
    DSBUFFERDESC bufferDesc;
    HRESULT result;
    IDirectSoundBuffer* tempBuffer;
    UCHAR* bufferPtr;
    ULONG bufferSize;
    CFileSystem* fs = ENGINE->GetFileSystem();
    CVirtualMachine* vm = ENGINE->GetVM();

    int channels;
    int sample_rate;
    short* output;
    count = stb_vorbis_decode_filename(fs->ResourcePath(oggPath), &channels, &sample_rate, &output);

    if (count == -1)
    {
        vm->PostError(CString::Format("Sound file: %s does not exist!", oggPath).Str());
        return;
    }

    if (channels != 2)
    {
        vm->PostError(CString::Format("Sound file: %s is invalid (needs 2 audio channels)!", oggPath).Str());
        return;
    }

    if (sample_rate != 44100)
    {
        vm->PostError(CString::Format("Sound file: %s is invalid (needs to be 44.1KHz)!", oggPath).Str());
        return;
    }

    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 2;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;
    *static_cast<WAVEFORMATEX*>(waveInfo) = waveFormat;

    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY;
    bufferDesc.dwBufferBytes = static_cast<DWORD>(count * channels * (waveFormat.wBitsPerSample / 8));
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = &waveFormat;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    result = AUDIO->GetDevice()->CreateSoundBuffer(&bufferDesc, &tempBuffer, nullptr);
    if (FAILED(result))
    {
        vm->PostError(CString::Format("Failed to create temporary sound buffer for file: %s!", oggPath).Str());
        return;
    }

    result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&*sndBuffer);
    if (FAILED(result))
    {
        tempBuffer->Release();
        vm->PostError(CString::Format("Failed to create sound buffer for file: %s!", oggPath).Str());
        return;
    }

    tempBuffer->Release();

    IDirectSoundBuffer8* snd = *sndBuffer;

    result = snd->Lock(0, bufferDesc.dwBufferBytes, (LPVOID*)&bufferPtr, static_cast<LPDWORD>(&bufferSize), nullptr,
                       nullptr, 0);
    if (FAILED(result))
    {
        vm->PostError(CString::Format("Failed to lock sound buffer for file: %s!", oggPath).Str());
        return;
    }

    *dataPtr = (UCHAR*)output;
    *dataSize = static_cast<ULONG>(count);
    memcpy(bufferPtr, (UCHAR*)output, bufferDesc.dwBufferBytes);

    result = snd->Unlock(static_cast<LPVOID>(bufferPtr), bufferSize, nullptr, 0);
    if (FAILED(result))
    {
        vm->PostError(CString::Format("Failed to unlock sound buffer for file: %s!", oggPath).Str());
    }
}

void CSoundLoader::LoadWAV3D(LPSTR wavPath, IDirectSoundBuffer8** sndBuffer, UCHAR** dataPtr, ULONG* dataSize,
                             LPVOID waveInfo)
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
    CFileSystem* fs = ENGINE->GetFileSystem();
    CVirtualMachine* vm = ENGINE->GetVM();

    fp = fs->OpenResource(wavPath);

    if (!fp)
    {
        vm->PostError(CString::Format("Sound file: %s does not exist!", wavPath).Str());
        return;
    }

    count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, fp);
    if (count != 1)
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid!", wavPath).Str());
        return;
    }

    if (strncmp("RIFF", waveFileHeader.chunkId, 4))
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (wrong ID)!", wavPath).Str());
        return;
    }

    if (strncmp("WAVE", waveFileHeader.format, 4))
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (wrong format)!", wavPath).Str());
        return;
    }

    if (strncmp("fmt ", waveFileHeader.subChunkId, 4))
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (wrong subID)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (wrong audio format)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.numChannels != 1)
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (needs 1 audio channel)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.sampleRate != 44100)
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (needs to be 44.1KHz)!", wavPath).Str());
        return;
    }

    if (waveFileHeader.bitsPerSample != 16)
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (needs to be 16-bit stereo)!", wavPath).Str());
        return;
    }

    if (strncmp("data", waveFileHeader.dataChunkId, 4))
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Sound file: %s is invalid (missing DATA marker)!", wavPath).Str());
        return;
    }

    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 1;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;
    *static_cast<WAVEFORMATEX*>(waveInfo) = waveFormat;

    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
    bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = &waveFormat;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    result = AUDIO->GetDevice()->CreateSoundBuffer(&bufferDesc, &tempBuffer, nullptr);
    if (FAILED(result))
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Failed to create temporary sound buffer for file: %s!", wavPath).Str());
        return;
    }

    result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&*sndBuffer);
    if (FAILED(result))
    {
        tempBuffer->Release();
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Failed to create sound buffer for file: %s!", wavPath).Str());
        return;
    }

    tempBuffer->Release();

    IDirectSoundBuffer8* snd = *sndBuffer;

    fseek(fp, sizeof(WAVEHEADER), SEEK_SET);
    waveData = new UCHAR[waveFileHeader.dataSize];
    if (!waveData)
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Failed to allocate data for sound buffer for file: %s!", wavPath).Str());
        return;
    }

    *dataPtr = waveData;
    *dataSize = waveFileHeader.dataSize;

    count = fread(waveData, 1, waveFileHeader.dataSize, fp);
    if (count != waveFileHeader.dataSize)
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Failed to read data for sound buffer for file: %s!", wavPath).Str());
        return;
    }

    fs->CloseResource(fp);

    result = snd->Lock(0, waveFileHeader.dataSize, (LPVOID*)&bufferPtr, static_cast<LPDWORD>(&bufferSize), nullptr,
                       nullptr, 0);
    if (FAILED(result))
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Failed to lock sound buffer for file: %s!", wavPath).Str());
        return;
    }

    memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

    result = snd->Unlock(static_cast<LPVOID>(bufferPtr), bufferSize, nullptr, 0);
    if (FAILED(result))
    {
        fs->CloseResource(fp);
        vm->PostError(CString::Format("Failed to unlock sound buffer for file: %s!", wavPath).Str());
    }
}

void CSoundLoader::OpenOGG(stb_vorbis** outDecoder, LPSTR path, IDirectSoundBuffer8** sndBuffer, HANDLE* events,
                           LPVOID waveInfo, DWORD* dataSize)
{
    WAVEFORMATEX waveFormat;
    DSBUFFERDESC bufferDesc;
    HRESULT result;
    IDirectSoundBuffer* tempBuffer;
    CFileSystem* fs = ENGINE->GetFileSystem();
    CVirtualMachine* vm = ENGINE->GetVM();

    int error;
    stb_vorbis* decoder = stb_vorbis_open_filename(fs->ResourcePath(path), &error, nullptr);
    stb_vorbis_info decoder_info = stb_vorbis_get_info(decoder);

    if (!decoder)
    {
        vm->PostError(CString::Format("Sound file: %s is invalid!", path).Str());
        return;
    }

    if (decoder_info.channels != 2)
    {
        vm->PostError(CString::Format("Sound file: %s is invalid (needs 2 audio channels)!", path).Str());
        return;
    }

    if (decoder_info.sample_rate != 44100)
    {
        vm->PostError(CString::Format("Sound file: %s is invalid (needs to be 44.1KHz)!", path).Str());
        return;
    }

    waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    waveFormat.nSamplesPerSec = 44100;
    waveFormat.wBitsPerSample = 16;
    waveFormat.nChannels = 2;
    waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
    waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
    waveFormat.cbSize = 0;
    *static_cast<WAVEFORMATEX*>(waveInfo) = waveFormat;
    *dataSize = static_cast<DWORD>(4096);

    bufferDesc.dwSize = sizeof(DSBUFFERDESC);
    bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLPOSITIONNOTIFY;
    bufferDesc.dwBufferBytes = 2 * waveFormat.nAvgBytesPerSec;
    bufferDesc.dwReserved = 0;
    bufferDesc.lpwfxFormat = &waveFormat;
    bufferDesc.guid3DAlgorithm = GUID_NULL;

    result = AUDIO->GetDevice()->CreateSoundBuffer(&bufferDesc, &tempBuffer, nullptr);
    if (FAILED(result))
    {
        vm->PostError(CString::Format("Failed to create temporary sound buffer for file: %s!", path).Str());
        return;
    }

    result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID*)&*sndBuffer);
    if (FAILED(result))
    {
        tempBuffer->Release();
        vm->PostError(CString::Format("Failed to create sound buffer for file: %s!", path).Str());
        return;
    }

    LPDIRECTSOUNDNOTIFY notify;
    result = tempBuffer->QueryInterface(IID_IDirectSoundNotify, (LPVOID*)&notify);
    if (FAILED(result))
    {
        tempBuffer->Release();
        vm->PostError(CString::Format("Failed to query for notify object for file: %s!", path).Str());
        return;
    }

    tempBuffer->Release();

    IDirectSoundBuffer8* snd = *sndBuffer;

    DSBPOSITIONNOTIFY notifyDesc[2];
    notifyDesc[0].dwOffset = waveFormat.nAvgBytesPerSec / 2 - 1;
    notifyDesc[1].dwOffset = 3 * waveFormat.nAvgBytesPerSec / 2 - 1;
    notifyDesc[0].hEventNotify = events[0];
    notifyDesc[1].hEventNotify = events[1];

    result = notify->SetNotificationPositions(2, notifyDesc);
    if (FAILED(result))
    {
        tempBuffer->Release();
        vm->PostError(CString::Format("Failed to set up notifications for file: %s!", path).Str());
        return;
    }

    notify->Release();

    *outDecoder = decoder;
}

void CSoundLoader::ResetBuffer(stb_vorbis* decoder)
{
    stb_vorbis_seek_start(decoder);
}

ULONG CSoundLoader::DecodeOGG(stb_vorbis* decoder, ULONG reqBytes, short** outData)
{
    short* data = new short[reqBytes / 2];
    int n = stb_vorbis_get_samples_short_interleaved(decoder, 2, data, reqBytes / 2);

    if (n == 0)
    {
        delete[] data;
        return 0;
    }

    *outData = data;

    return n * 2;
}

DWORD CSoundLoader::TellOGG(stb_vorbis* decoder)
{
    return static_cast<DWORD>(stb_vorbis_get_file_offset(decoder));
}

void CSoundLoader::CloseOGG(stb_vorbis* decoder)
{
    stb_vorbis_close(decoder);
}
