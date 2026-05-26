#pragma once
#include <string>
#include <Windows.h>

//Main
inline const WCHAR* S_MAIN_CLASSNAME = L"ArkanoidClass";
inline const WCHAR* S_MAIN_WINDOWNAME = L"ArkanoidClass - D'Ambrosio";
inline const WCHAR* S_ERROR = L"Error";
inline const WCHAR* S_ERROR_INIT = L"Initialization failed";

//Game
inline const std::wstring S_SHADERS_PATH = L"shaders\\";
inline const LPCSTR S_ERROR_INIT_RENDERER = "FAIL: Renderer DX11\n";
inline const LPCSTR S_ERROR_INIT_RENDERER2D = "FAIL: Renderer2D (shader?)\n";
inline const LPCSTR S_ERROR_INIT_TEXTRENDERER = "FAIL: TextRenderer (D2D)\n";
inline const LPCSTR S_ERROR_INIT_LIGHTBALLRENDERER = "FAIL: LightBallMode (shader?)\n";
inline const LPCSTR S_ERROR_INIT_AUDIOMANAGER = "FAIL: AudioManager\n";

//LightBallRenderer
inline const LPCSTR S_ERROR_LBR_INIT_CREATERTV = "FAIL: LightBallRenderer::Init > CreateRenderTarget\n";
inline const LPCSTR S_ERROR_LBR_INIT_LOADSHADER = "FAIL: LightBallRenderer::Init > Shader\n";
inline const LPCSTR S_ERROR_LBR_INIT_CREATEBUFFER = "FAIL: LightBallRenderer::Init > CreateBuffer\n";
inline const LPCSTR S_ERROR_LBR_INIT_CREATESAMPLER = "FAIL: LightBallRenderer::Init > CreateSamplerState\n";
inline const LPCSTR S_ERROR_LBR_CREATERT_TEXTURE = "FAIL: LightBallRenderer::CreateRenderTarget > CreateTexture2D\n";
inline const LPCSTR S_ERROR_LBR_CREATERT_RTV = "FAIL: LightBallRenderer::CreateRenderTarget > CreateRenderTargetView\n";
inline const LPCSTR S_ERROR_LBR_CREATERT_SRV = "FAIL: LightBallRenderer::CreateRenderTarget > CreateShaderResourceView\n";

//Sound
inline const LPCSTR S_ERROR_SOUND_INIT_XAUDIO2 = "FAIL: SoundManager::Init > XAudio2Create\n";
inline const LPCSTR S_ERROR_SOUND_INIT_MASTERINGVOICE = "FAIL: SoundManager::Init > CreateMasteringVoice\n";
inline const LPCSTR S_ERROR_SOUND_INIT_LOAD = "FAIL: SoundManager::Init > Load (Una o piu' clip non caricate, audio parziale)\n";
inline const LPCSTR S_ERROR_SOUND_LOAD = "FAIL: SoundManager::LoadAll > CreateSourceVoice\n";
inline const LPCSTR S_ERROR_SOUND_PLAY = "FAIL: SoundManager::LoadAll > SubmitSourceBuffer\n";
inline std::wstring S_ERROR_SOUND_PATHNOTFOUND(const std::wstring& path) { return L"FAIL: SoundClip::Load > File non trovato: " + path + L"\n";  }
inline std::wstring S_ERROR_SOUND_RIFFNOTVALID(const std::wstring& path) { return L"FAIL: SoundClip::Load > Formato RIFF non valido: " + path + L"\n";  }
inline std::wstring S_ERROR_SOUND_CHUNKERROR(const std::wstring& path) { return L"FAIL: SoundClip::Load > Chunk fmt/data mancante: " + path + L"\n";  }

//UI
inline const std::wstring S_AIRACKET = L"AI racket";
inline const std::wstring S_AIRACKET_ONLYBALL = L"Only balls";
inline const std::wstring S_AIRACKET_ADAPTIVE = L"Adaptive";
inline const std::wstring S_CONTINUE = L"CONTINUE (C)";
inline const std::wstring S_GAMEOVER = L"GAME OVER";
inline const std::wstring S_GAMEOVER_RAPIDCOMMAND = L"R / Q / ESC";
inline const std::wstring S_GAMESETTINGS = L"Game settings";
inline const std::wstring S_GE = L">";
inline const std::wstring S_LE = L"<";
inline const std::wstring S_LIGHTBALL = L"Light ball mode";
inline const std::wstring S_OFF = L"Off";
inline const std::wstring S_ON = L"On";
inline const std::wstring S_PAUSE = L"PAUSE";
inline const std::wstring S_PAUSE_RAPIDCOMMAND = L"C / R / Q / ESC";
inline const std::wstring S_PLAY = L"PLAY (ENTER)";
inline const std::wstring S_QUICKRESTART = L"QUICK RESTART (Q)";
inline const std::wstring S_QUIT = L"QUIT (ESC)";
inline const std::wstring S_RESTART = L"RESTART (R)";
inline const std::wstring S_TITLE = L"ARKANOID";
inline const std::wstring S_TRAJECTORY = L"Ball trajectory";
inline const std::wstring S_TRAJECTORY_MOSTDANGEROUS = L"Most dangerous";
inline const std::wstring S_TRAJECTORY_ALL = L"All";

inline std::wstring S_LEVEL(int level) { return L"LEVEL  " + std::to_wstring(level); }
inline std::wstring S_SCORE(int score) { return L"SCORE  " + std::to_wstring(score); }






