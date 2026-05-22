#pragma once
#define NOMINMAX
#include <string>
#include <vector>
#include <Windows.h>

//Rappresenta un singolo file audio caricato in memoria
//Si occupa solo di aprire il .wav dal disco e tenerlo in memoria
class SoundClip
{
public:
    SoundClip() = default;
    ~SoundClip() = default;

    //Rule of five: se definisco distruttore, devo definire anche copy e move (FC++, lezione 7)
    SoundClip(const SoundClip&) = delete;
    SoundClip& operator=(const SoundClip&) = delete;
    SoundClip(SoundClip&&) = default;
    SoundClip& operator=(SoundClip&&) = default;

    //Apre il file .wav e lo parsa
    bool Load(const std::wstring& path);

    const WAVEFORMATEX& GetFormat() const { return fmt; }
    const BYTE* GetData() const { return data.data(); }
    UINT32 GetDataSize() const { return static_cast<UINT32>(data.size()); }
    bool IsLoaded() const { return loaded; }

private:
    WAVEFORMATEX fmt = {};
    std::vector<BYTE> data;
    bool loaded = false;
};