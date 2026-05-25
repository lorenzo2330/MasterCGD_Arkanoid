#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <windows.h>

class Renderer
{
public:
    Renderer() = default;
    ~Renderer() { Shutdown(); }

    //Rule of five: se definisco distruttore, devo definire anche copy e move (FC++, lezione 7)
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = delete;
    Renderer& operator=(Renderer&&) = delete;

	bool Init(HWND hwnd);   //Crea device, swap chain, render target view, viewport
	void BeginFrame();      //Pulisce lo schermo a inizio frame
    void EndFrame();        //Invoca swapChain->Present(...) una volta che il frame è pronto
	void Shutdown();        //Rilascia risorse

    ID3D11Device* GetDevice()  const { return device; }
    ID3D11DeviceContext* GetContext() const { return context; }
    IDXGISwapChain* GetSwapChain()  const { return swapChain; }
    ID3D11RenderTargetView* GetRenderTargetView() const { return rtv; }

private:
    ID3D11Device* device = nullptr;     
    ID3D11DeviceContext* context = nullptr;
    IDXGISwapChain* swapChain = nullptr;
    ID3D11RenderTargetView* rtv = nullptr;
};