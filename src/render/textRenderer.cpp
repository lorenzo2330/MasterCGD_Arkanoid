#define NOMINMAX
#pragma comment(lib, "d2d1.lib")        
#pragma comment(lib, "dwrite.lib")      
#include "../colors.h"
#include "../data.h"
#include "textRenderer.h"
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_1helper.h>
#include <d2d1helper.h>
#include <d3d11.h>
#include <dcommon.h>
#include <dxgi.h>
#include <dxgiformat.h>
#include <dwrite.h>
#include <string>
#include <Windows.h>

bool TextRenderer::Init(ID3D11Device* d3dDevice, IDXGISwapChain* swapChain)
{
    IDXGIDevice* dxgiDevice = nullptr;
    d3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if (!dxgiDevice) return false;
    
    //Creazione a catena di oggetti

    //D2D1 Factory
    D2D1_FACTORY_OPTIONS opts = {};
    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), &opts, reinterpret_cast<void**>(&d2dFactory));
    if (FAILED(hr)) { dxgiDevice->Release(); return false; }

    //D2D1 Device
    hr = d2dFactory->CreateDevice(dxgiDevice, &d2dDevice);
    if (FAILED(hr)) { dxgiDevice->Release(); return false; }

    //D2D Device Context
    hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);
    if (FAILED(hr)) { dxgiDevice->Release(); return false; }

    //Backbuffer
    if (!CreateBitmapFromSwapChain(swapChain)) { dxgiDevice->Release(); return false; }

    //DirectWrite Factory
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&dwFactory));
    
    dxgiDevice->Release();

    return SUCCEEDED(hr);
}

bool TextRenderer::CreateBitmapFromSwapChain(IDXGISwapChain* swapChain)
{
    //Crea il backbuffer

    IDXGISurface* surface = nullptr;
    HRESULT hr = swapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(&surface));
    if (FAILED(hr)) return false;

    // Proprietà della bitmap: pixel format compatibile DX11 + D2D
    D2D1_BITMAP_PROPERTIES1 props = D2D1::BitmapProperties1(
        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    );

    hr = d2dContext->CreateBitmapFromDxgiSurface(surface, props, &d2dTarget);
    surface->Release();
    if (FAILED(hr)) return false;

    d2dContext->SetTarget(d2dTarget);
    return true;

}

void TextRenderer::BeginDraw() { d2dContext->BeginDraw(); }

void TextRenderer::EndDraw() { d2dContext->EndDraw(); }


void TextRenderer::DrawText(const std::wstring& text, float x, float y, float fontSize, Color c)
{
    IDWriteTextFormat* fmt = MakeFormat(fontSize);
    ID2D1SolidColorBrush* brush = MakeBrush(c);
    if (!fmt || !brush) { if (fmt) fmt->Release(); if (brush) brush->Release(); return; }

    //Rettangolo dove andrà a disegnare
    D2D1_RECT_F rect = D2D1::RectF(x, y, SCREEN_WIDTH, SCREEN_HEIGHT);

    d2dContext->DrawText(text.c_str(), static_cast<UINT32>(text.size()), fmt, rect, brush);
    fmt->Release();
    brush->Release();
}

void TextRenderer::DrawCenteredText(const std::wstring& text, float x, float y, float width, float fontSize, Color c)
{
    IDWriteTextFormat* fmt = MakeFormat(fontSize);
    ID2D1SolidColorBrush* brush = MakeBrush(c);
    if (!fmt || !brush) { if (fmt) fmt->Release(); if (brush) brush->Release(); return; }

    fmt->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);

    D2D1_RECT_F rect = D2D1::RectF(x, y, x + width, SCREEN_HEIGHT);

    d2dContext->DrawText(text.c_str(), static_cast<UINT32>(text.size()), fmt, rect, brush);
    fmt->Release();
    brush->Release();
}

ID2D1SolidColorBrush* TextRenderer::MakeBrush(Color c)
{
    //Helper per creare un pannello
    ID2D1SolidColorBrush* brush = nullptr;
    d2dContext->CreateSolidColorBrush(D2D1::ColorF(c.r, c.g, c.b, c.a), &brush);
    return brush;
}

IDWriteTextFormat* TextRenderer::MakeFormat(float fontSize)
{
	//Helper per creare un testo
    IDWriteTextFormat* fmt = nullptr;
    dwFactory->CreateTextFormat(
        L"Segoe UI",                //font di sistema, sempre disponibile su Windows
        nullptr,
        DWRITE_FONT_WEIGHT_BOLD,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        fontSize,
        L"en-us",
        &fmt);
    return fmt;
}

void TextRenderer::Shutdown()
{
    if (!d2dDevice) return;    //Per evitare che venga chiamato più volte (nel distruttore e in Game::Shutdown)
    if (d2dTarget) { d2dTarget->Release();  d2dTarget = nullptr; }
    if (d2dContext) { d2dContext->Release(); d2dContext = nullptr; }
    if (d2dDevice) { d2dDevice->Release();  d2dDevice = nullptr; }
    if (d2dFactory) { d2dFactory->Release(); d2dFactory = nullptr; }
    if (dwFactory) { dwFactory->Release();  dwFactory = nullptr; }
}