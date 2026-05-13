#pragma once
#include "../colors.h"
#include <d3d11.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <dxgi.h>
#include <string>
#include <windows.h>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dxgi.lib")

class TextRenderer
{
public:
    TextRenderer() = default;
    ~TextRenderer() { Shutdown(); }

    //Rule of five: se definisco distruttore, devo definire anche copy e move (FC++, lezione 7)
    TextRenderer(const TextRenderer&) = delete;
    TextRenderer& operator=(const TextRenderer&) = delete;
    TextRenderer(TextRenderer&&) = delete;
    TextRenderer& operator=(TextRenderer&&) = delete;

    // dxgiDevice ricavato da ID3D11Device::QueryInterface<IDXGIDevice>
    bool Init(ID3D11Device* d3dDevice, IDXGISwapChain* swapChain); //Configura scrittura di testo

    void BeginDraw();           //Apre la sessione di scrittura (necessità di D2D1)
    void EndDraw();             //Chiude la sessione di scrittura (necessità di D2D1)

    //Prepara il testo e la posizione, per poi farlo disegnare tramite d2dContext->DrawText()
    void DrawText(const std::wstring& text, float x, float y, float fontSize, Color c);

    //Posiziona il testo al centro di una certa area e poi invoca DrawText() per disegnarlo
    void DrawCenteredText(const std::wstring& text, float x, float y, float width, float fontSize, Color c);

    void Shutdown();

private:
    //Elementi D2D1 (dipendenze a cascata, ogni oggetto dipende da quello precedente)
    ID2D1Factory1* d2dFactory = nullptr;      //Crea il device
    ID2D1Device* d2dDevice = nullptr;         //Device usato
    ID2D1DeviceContext* d2dContext = nullptr; //L'oggetto che disegna effettivamente
    ID2D1Bitmap1* d2dTarget = nullptr;        //Backbuffer

    //Elementi DirectWrite
    IDWriteFactory* dwFactory = nullptr;      //Crea oggetti per disegnare testo

    bool CreateBitmapFromSwapChain(IDXGISwapChain* swapChain);  //Crea il backbuffer
    ID2D1SolidColorBrush* MakeBrush(Color c);                   //Helper per creare un pannello
    IDWriteTextFormat* MakeFormat(float fontSize);              //Helper per creare un testo
};