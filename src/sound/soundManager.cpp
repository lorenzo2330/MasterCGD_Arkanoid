#pragma comment(lib, "xaudio2.lib")
#include "../data.h"
#include "soundBank.h"
#include "SoundManager.h"
#include <xaudio2.h>
#include <Windows.h>



bool SoundManager::Init()
{
    //Inizializzazione XAudio2
    if (FAILED(XAudio2Create(xaudio2.GetAddressOf(), 0, XAUDIO2_DEFAULT_PROCESSOR))){ OutputDebugStringA("[SoundManager] XAudio2Create fallito.\n"); return false; }

    //Inizializzazione MasteringVoice
    if (FAILED(xaudio2->CreateMasteringVoice(&masterVoice))) { OutputDebugStringA("[SoundManager] CreateMasteringVoice fallito.\n"); return false; }

    //Caricamento dei file di audio
    if (!LoadAll()) { OutputDebugStringA("[SoundManager] Uno o piu' clip non caricati. Audio parziale.\n"); }

    initialized = true;
    
    return true;
}

// ---------------------------------------------------------------------------
bool SoundManager::LoadAll()
{
    bool allLoaded = true;

    for (size_t i = 0; i < static_cast<size_t>(SoundID::COUNT); ++i)
    {
        SoundEntry& entry = sounds[i];
        entry.nextVoice = 0;

        // --- Carica il clip dal disco ---
        if (!entry.clip.Load(soundPaths[i]))
        {
            allLoaded = false;
            continue;   // Voci non create: Play ignorerà questo suono
        }

        // --- Crea il pool di SourceVoice ---
        entry.voices.resize(MAX_VOICES_TOGHETHER, nullptr);

        for (int v = 0; v < MAX_VOICES_TOGHETHER; ++v)
        {
            HRESULT hr = xaudio2->CreateSourceVoice(
                &entry.voices[v],
                &entry.clip.GetFormat()
            );
            if (FAILED(hr))
            {
                OutputDebugStringA("[SoundManager] CreateSourceVoice fallito per un suono.\n");
                entry.voices[v] = nullptr;
                allLoaded = false;
            }
        }
    }

    return allLoaded;
}

// ---------------------------------------------------------------------------
void SoundManager::Play(SoundID id)
{
    if (!initialized || muted) return;

    //Ottiene un riferimento alla SoundEntry relativa al suono con l'id richiesto
    SoundEntry& entry = sounds[static_cast<size_t>(id)];

    //Controlla se la clip corrispondente è stata caricata, in caso contrario semplicemente non la riproduce
    if (!entry.clip.IsLoaded() || entry.voices.empty()) return;

    //Round-robin sul pool di voci (per eseguirle tutte in contemporanea senza che si ostacolino)
    IXAudio2SourceVoice* voice = entry.voices[entry.nextVoice];
    entry.nextVoice = (entry.nextVoice + 1) % MAX_VOICES_TOGHETHER;

    //Se la voce selezionata dal pool non è stata inizializzata correttamente
    if (!voice) return;

    //Resetta la voce, qualora fosse ancora in riproduzione da prima
    voice->Stop();
    voice->FlushSourceBuffers();

    //Prepara il buffer per l'audio
    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = entry.clip.GetDataSize();
    buffer.pAudioData = entry.clip.GetData();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    //Carica il buffer in memoria
    if (FAILED(voice->SubmitSourceBuffer(&buffer))) { OutputDebugStringA("[SoundManager] SubmitSourceBuffer fallito.\n"); return; }

    //Avvia la riproduzione vera a propria (simile a Draw())
    voice->Start();
}

void SoundManager::SetMasterVolume(float v) { volume = v; if (masterVoice && !muted){ masterVoice->SetVolume(volume); } }

void SoundManager::SetMuted(bool m) { muted = m; if (masterVoice) { masterVoice->SetVolume(muted ? 0.0f : volume); } }

void SoundManager::Shutdown()
{
    if (!initialized) return;

    //Distrugge le SourceVoice
    for (auto& entry : sounds) { 
        for (IXAudio2SourceVoice* v : entry.voices) { if (v) { v->Stop(); v->DestroyVoice(); } } 
        entry.voices.clear();
    }

    //Distrugge la MasteringVoice
    if (masterVoice) { masterVoice->DestroyVoice(); masterVoice = nullptr; }

    //Rilascia IXAudio2
    xaudio2.Reset();

    initialized = false;
}