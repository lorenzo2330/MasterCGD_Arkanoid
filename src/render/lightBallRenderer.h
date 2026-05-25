#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../data.h"
#include "renderer.h"
#include "shader.h"
#include <string>



//Struttura condivisa tra CPU e GPU per capire come gestire la luminosità
struct BallLightData
{
    DirectX::XMFLOAT4 ballPositions[LIGHTBALL_MAX_BALL], bonusPositions[LIGHTBALL_MAX_BONUS];
    float ballCount, bonusCount, ballLightRadius, racketLightRadius, bonusLightRadius, ballPower, racketPower, bonusPower;
    DirectX::XMFLOAT4 racketPosition;
};
/*
* Nota: la sizeof deve essere multipla di 16 (per necessità di GPU, che legge in blocchi da 16 byte)
*   in questo caso è ok 128 * 2 + 4 * 8 + 16 = 176 % 16 = 0
*       -ballPosition e bonusPositions sono entrambi = 16 * 8 = 128 byte (LIGHTBALL_MAX_BALL = 8)
*       -ballCount, bonusCount, ballLightRadius, racketLightRadius, bonusLightRadius, ballPower, racketPower e bonusPower sono tutti 4 byte
*       -racketPosition è 16 byte
*   non fosse stato così, si sarebbe dovuto aggiungere del padding in modo da avere esattamente un multiplo di 16
*/

class LightBallRenderer
{
public:
    LightBallRenderer() = default;
    ~LightBallRenderer() { Shutdown(); }

    //Rule of five: se definisco distruttore, devo definire anche copy e move (FC++, lezione 7)
    LightBallRenderer(const LightBallRenderer&) = delete;
    LightBallRenderer& operator=(const LightBallRenderer&) = delete;
    LightBallRenderer(const LightBallRenderer&&) = delete;
    LightBallRenderer& operator=(const LightBallRenderer&&) = delete;

    bool Init(Renderer& renderer, const std::wstring& shaderPath);

    //Rilascio delle risorse
    void Shutdown();

    //Anzichè direttamente sul backbuffer, reindirizza il rendering su sceneRTV ("backbackbuffer")
    void BeginFrame(Renderer& renderer);

    //Aggiorna il constant buffer, poi il ps lo legge, calcola i colori di ogni pixel e li scrive sul vero backBuffer
    void EndFrame(Renderer& renderer, const BallLightData& lightData);

    //Indica se il sistema è stato inizializzato correttamente
    bool IsReady() const { return ready; }

private:
    bool CreateRenderTarget(ID3D11Device* device, ID3D11Texture2D** outTex, ID3D11RenderTargetView** outRTV, ID3D11ShaderResourceView** outSRV);

    Shader shader;

    ID3D11Texture2D* bbBuffer = nullptr;            //Texture in "doppia lettura" usata come "backBackBuffer" // texture 2D sottostante
    ID3D11RenderTargetView* sceneRTV = nullptr;     //View per usare bbBuffer in scrittura (in BeginFrame)
    ID3D11ShaderResourceView* sceneSRV = nullptr;   //View per usare bbBuffer in lettura (in EndFrame)

    ID3D11Buffer* lightCB = nullptr;                //Constant buffer: invia i dati delle palline allo shader

    ID3D11SamplerState* sampler = nullptr;          //Sampler: indica come leggere la texture

    bool ready = false;
};