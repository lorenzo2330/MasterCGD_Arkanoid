#include "LightBallRenderer.h"
#include "renderer.h"   // per Renderer::GetDevice(), GetContext(), GetBackbufferRTV()
#include "../data.h"
#include <d3d11.h>
#include <d3dcommon.h>
#include <dxgiformat.h>
#include <string>
#include <string.h>
#include <Windows.h>

#pragma comment(lib, "d3dcompiler.lib")


/*  Idea:
* -Anzichè usare solamente un backbuffer e un frontbuffer, usiamo anche un "backbackbuffer" (fase intermedia prima di arrivare al backbuffer)
* 
* -FASE 1
* -Si passa a render2d una texture intermedia, sulla quale disegnerà palline, mattoncini, racchetta, …
* --Questa texture intermedia, chiamata bbBuffer, è uguale (dal punto di vista visivo) al gioco normale nelle altre modalità (senza "filtro di buio")
* 
* -FASE 2
* -Il pixel shader legge bbBuffer e per ogni pixel decide quanto illuminarlo (in base alla distanza da palline, racchetta, …)
* --Fa una "semplice" moltiplicazione: colore_finale = colore_iniziale * luminosità_contestuale
* --Il risultato viene direttamente scritto sul vero backbuffer e da li mostrato a schermo
*/

bool LightBallRenderer::Init(Renderer& renderer, const std::wstring& shaderDir)
{
    ID3D11Device* device = renderer.GetDevice();

    //Crea il "backbackbuffer" (buffer intermedio di supporto al backbuffer)
    if (!CreateRenderTarget(device, &bbBuffer, &sceneRTV, &sceneSRV)) { return ERR("LightBallRenderer::Init - FAIL: CreateRenderTarget (scene)\n"); }

    //Compilazione degli shader
    if (!shader.Load(device, shaderDir + L"vs_fullscreen.hlsl", shaderDir + L"ps_lightball.hlsl")) return ERR("LightBallRenderer::Init - FAIL: shader\n");

    //Constant buffer per passarsi le informazioni su palline (n, pos, ...), racchetta, bonus, ...
    D3D11_BUFFER_DESC cbd = {};
    cbd.ByteWidth = sizeof(BallLightData);
    cbd.Usage = D3D11_USAGE_DYNAMIC;
    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    if (FAILED(device->CreateBuffer(&cbd, nullptr, &lightCB))) { return ERR("LightBallRenderer::Init - FAIL: CreateBuffer (lightCB)\n"); }

    //Sampler (indica come leggere la texture)
    D3D11_SAMPLER_DESC sd = {};
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    if (FAILED(device->CreateSamplerState(&sd, &sampler))) { return ERR("LightBallRenderer::Init - FAIL: CreateSamplerState\n"); }

    ready = true;
    return true;
}

void LightBallRenderer::Shutdown()
{
    //Rilascio delle risorse 
    if (!ready) return;

    if (sampler) { sampler->Release(); sampler = nullptr; }
    if (lightCB) { lightCB->Release(); lightCB = nullptr; }
    shader.Release();
    if (sceneSRV) { sceneSRV->Release(); sceneSRV = nullptr; }
    if (sceneRTV) { sceneRTV->Release(); sceneRTV = nullptr; }
    if (bbBuffer) { bbBuffer->Release(); bbBuffer = nullptr; }

    ready = false;
}

//Anzichè direttamente sul backbuffer, reindirizza il rendering su bbBuffer
void LightBallRenderer::BeginFrame(Renderer& renderer)
{
    ID3D11DeviceContext* ctx = renderer.GetContext();

    //Sfondo nero
    float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
    ctx->ClearRenderTargetView(sceneRTV, clearColor);

    //Imposta sceneRTV (bbBuffer in modalità scrittura) come destinazione delle successive drawCall
    ctx->OMSetRenderTargets(1, &sceneRTV, nullptr);

    //Imposta esplicitamente il nuovo viewport (non viene fatto automaticamente, bisogna farlo manualmente)
    D3D11_VIEWPORT vp = {};
    vp.Width = SCREEN_WIDTH;
    vp.Height = SCREEN_HEIGHT;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    ctx->RSSetViewports(1, &vp);
}

//Aggiorna il constant buffer, poi il ps lo legge, calcola i colori di ogni pixel e li scrive sul vero backBuffer
void LightBallRenderer::EndFrame(Renderer& renderer, const BallLightData& lightData)
{
    ID3D11DeviceContext* ctx = renderer.GetContext();

    //Aggiorna il constant buffer con le posizioni delle palline
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    if (SUCCEEDED(ctx->Map(lightCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
    {
        memcpy(mapped.pData, &lightData, sizeof(BallLightData));
        ctx->Unmap(lightCB, 0);
    }

    //Imposta nuovamente il vero backbuffer come destinazione delle successive drawCall
    ID3D11RenderTargetView* backbufferRTV = renderer.GetRenderTargetView();
    ctx->OMSetRenderTargets(1, &backbufferRTV, nullptr);

    //Bind delle risorse (serve per il pixel shader)
    ctx->PSSetShaderResources(0, 1, &sceneSRV);
    ctx->PSSetConstantBuffers(0, 1, &lightCB);
    ctx->PSSetSamplers(0, 1, &sampler);

    //Disegna un triangolo che copre tutto lo schermo, serve solo come pretesto per far operare il pixel shader
    //**Tecnica chiamata fullscreen pass**
    ctx->IASetInputLayout(nullptr);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    shader.Bind(ctx);
    ctx->Draw(3, 0);

    //Pulizia (evita warning: "resource still bound as SRV when set as RTV")
    ID3D11ShaderResourceView* nullSRV = nullptr;
    ctx->PSSetShaderResources(0, 1, &nullSRV);
}

//Crea bbBuffer, rendendola utilizzabile in doppio modo (come destinazione in BeginFrame e come sorgente in EndFrame)
bool LightBallRenderer::CreateRenderTarget(ID3D11Device* device, ID3D11Texture2D** outTex, ID3D11RenderTargetView** outRTV, ID3D11ShaderResourceView** outSRV)
{
    //Creazione della texture
    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = static_cast<UINT>(SCREEN_WIDTH);
    texDesc.Height = static_cast<UINT>(SCREEN_HEIGHT);
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texDesc.SampleDesc.Count = 1;               
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    if (FAILED(device->CreateTexture2D(&texDesc, nullptr, outTex))){ return ERR("LightBallRenderer::CreateRenderTarget - FAIL: CreateTexture2D\n"); }

    //Setting per modalità "destinazione" (BeginFrame) -> RenderTargetView
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = texDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    if (FAILED(device->CreateRenderTargetView(*outTex, &rtvDesc, outRTV))) { return ERR("LightBallRenderer::CreateRenderTarget - FAIL: CreateRenderTargetView\n"); }

    //Setting per modalità "sorgente" (EndFrame) -> ShaderResourceView
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = texDesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    if (FAILED(device->CreateShaderResourceView(*outTex, &srvDesc, outSRV))) { return ERR("LightBallRenderer::CreateRenderTarget - FAIL: CreateShaderResourceView\n"); }

    return true;
}
