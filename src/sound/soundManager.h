#pragma once
#define NOMINMAX
#include "soundClip.h"
#include "soundBank.h"
#include <xaudio2.h>
#include <array>
#include <vector>
#include <wrl/client.h>

class SoundManager
{
public:
    //Singleton: unica istanza globale -> evita di dover avere un SoundManager in ogni file
    static SoundManager& Get() { static SoundManager instance; return instance; }

    SoundManager(const SoundManager&) = delete;
    SoundManager& operator=(const SoundManager&) = delete;

    //Istanzia XAudio2 ed il Mastering Voice 
    bool Init();

    //Rilascia le risorse
    void Shutdown();

    //Riproduce il suono relativo all'id ricevuto
    void Play(SoundID id);

    //Metodi per gestire il volume in maniera generale
    void SetMasterVolume(float volume);
    void SetMuted(bool muted);
    bool IsMuted() const { return muted; }

private:
    SoundManager() = default;
    ~SoundManager() = default;

    //Per ogni entry in SoundID, carica asset e clip corrispondente (SoundClip::Load())
    bool LoadAll();

    //Struttura per ogni singolo suono
    struct SoundEntry { SoundClip clip; std::vector<IXAudio2SourceVoice*> voices; int nextVoice = 0; };

    //Punto d'ingresso del sistema XAudio2 (come ID3D11Device per DirectX)
    Microsoft::WRL::ComPtr<IXAudio2> xaudio2;
    //Punto d'uscita verso il dispositivo di audio fisico (come RenderTargetView in DirectX)
    IXAudio2MasteringVoice* masterVoice = nullptr;

    //Array (a dimensione fissa) contenente tutte le entry di SoundID
    std::array<SoundEntry, static_cast<size_t>(SoundID::COUNT)> sounds;

    bool initialized = false, muted = false;
    float volume = 1.0f;
};