#include "renderer.h"
#include "../data.h"
#include "../colors.h"
#include <d3d11.h>
#include <d3dcommon.h>
#include <dxgi.h>
#include <dxgiformat.h>
#include <Windows.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

bool Renderer::Init(HWND hwnd)
{
    //Slides 13-15

	//Definizione swap chain
    DXGI_SWAP_CHAIN_DESC sd = {};
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;                                 //2 per evitare tearing
    sd.BufferDesc.Width = static_cast<int>(SCREEN_WIDTH);
    sd.BufferDesc.Height = static_cast<int>(SCREEN_HEIGHT);
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;      //scambia i puntatori ai buffer

	//Creazione device, swap chain e context
    D3D_FEATURE_LEVEL featureLevel;
    HRESULT test = D3D11CreateDeviceAndSwapChain(
        nullptr, 
        D3D_DRIVER_TYPE_HARDWARE, 
        nullptr, 
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,   //Necessario per D2D interop (Hud)
        nullptr, 
        0, 
        D3D11_SDK_VERSION, 
        &sd, 
        &swapChain, 
        &device, 
        &featureLevel, 
        &context
    );

    if (FAILED(test)) return false;

	//Creazione back buffer e render target view
    ID3D11Texture2D* backBuffer = nullptr;
    if (FAILED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer))) return false;
    
    HRESULT hr = device->CreateRenderTargetView(backBuffer, nullptr, &rtv);
    backBuffer->Release();
	if (FAILED(hr)) return false;

    //"numero di render target, puntatore back buffer, puntatore al depth stencil"
    context->OMSetRenderTargets(1, &rtv, nullptr);

	//Setting viewport
    D3D11_VIEWPORT vp = {};
    vp.Width = SCREEN_WIDTH;
    vp.Height = SCREEN_HEIGHT;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    context->RSSetViewports(1, &vp);

    return true;
}

void Renderer::BeginFrame()
{
	const float clearColor[4] = { COLOR_GAME_BACKGROUND.r, COLOR_GAME_BACKGROUND.g, COLOR_GAME_BACKGROUND.b, COLOR_GAME_BACKGROUND.a };
	//Slide 16
    //Pulisce lo schermo col colore clearColor
    context->ClearRenderTargetView(rtv, clearColor);
    context->OMSetRenderTargets(1, &rtv, nullptr);
}

void Renderer::EndFrame()
{
    //Slide 16
    //Mostra il backbuffer
    swapChain->Present(1, 0); //(1, 0) per evitare tearing
}

void Renderer::Shutdown()
{
	if (!device) return;    //Per evitare che venga chiamato più volte (nel distruttore e in Game::Shutdown)
    //Slide 17
    //Rilascio delle risorse
    if (rtv) { rtv->Release(); rtv = nullptr; }
    if (swapChain) { swapChain->Release(); swapChain = nullptr; }
    if (context) { context->Release(); context = nullptr; }
    if (device) { device->Release(); device = nullptr; }
}