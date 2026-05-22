#define NOMINMAX
#include "SoundClip.h"
#include <algorithm>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <string>
#include <Windows.h>

//Strutture per il parsing dei file .wav
#pragma pack(push, 1)
struct RiffChunkHeader { char id[4]; uint32_t size; };
struct RiffHeader { RiffChunkHeader chunk; char format[4]; };
#pragma pack(pop)

//Apre il file .wav e lo parsa
bool SoundClip::Load(const std::wstring& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) { OutputDebugStringW((L"[SoundClip] File non trovato: " + path + L"\n").c_str()); return false; }

    // ---- RIFF header ----
    RiffHeader riff{};
    file.read(reinterpret_cast<char*>(&riff), sizeof(riff));
    if (std::strncmp(riff.chunk.id, "RIFF", 4) != 0 || std::strncmp(riff.format, "WAVE", 4) != 0)
    {
        OutputDebugStringW((L"[SoundClip] Formato RIFF non valido: " + path + L"\n").c_str());
        return false;
    }

    bool fmtFound = false;
    bool dataFound = false;

    // ---- Scansione dei chunk ----
    while (!file.eof())
    {
        RiffChunkHeader hdr{};
        if (!file.read(reinterpret_cast<char*>(&hdr), sizeof(hdr)))
            break;

        if (std::strncmp(hdr.id, "fmt ", 4) == 0)
        {
            // Legge solo i primi 16 byte (PCM standard); ignora l'eventuale extension
            WAVEFORMATEX searchFmt{};
            uint32_t readSize = std::min(hdr.size, static_cast<uint32_t>(sizeof(WAVEFORMATEX)));
            file.read(reinterpret_cast<char*>(&searchFmt), readSize);

            // Se il chunk è più grande di quanto abbiamo letto, salta il resto
            if (hdr.size > readSize) { file.seekg(static_cast<std::streamoff>(hdr.size) - static_cast<std::streamoff>(readSize), std::ios::cur); }

            fmt = searchFmt;
            fmt.cbSize = 0;   // cbSize non ha senso per PCM puro
            fmtFound = true;
        }
        else if (std::strncmp(hdr.id, "data", 4) == 0)
        {
            data.resize(hdr.size);
            file.read(reinterpret_cast<char*>(data.data()), hdr.size);
            dataFound = true;
        }
        else
        {
            // Chunk sconosciuto (LIST, cue, …): salta
            // I chunk RIFF hanno dimensioni allineate a 2 byte
            uint32_t skip = (hdr.size + 1) & ~1u;
            file.seekg(skip, std::ios::cur);
        }

        if (fmtFound && dataFound) break;
    }

    if (!fmtFound || !dataFound) { OutputDebugStringW((L"[SoundClip] Chunk fmt/data mancante: " + path + L"\n").c_str()); return false; }

    loaded = true;
    return true;
}