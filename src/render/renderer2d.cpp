#define NOMINMAX
#include "renderer2d.h"
#include "../colors.h"
#include <algorithm>
#include <cstdlib>
#include <d3d11.h>
#include <d3dcommon.h>
#include <dxgiformat.h>
#include <string>
#include <string.h>
#include <Windows.h>

bool Renderer2D::Init(ID3D11Device* device, ID3D11DeviceContext* ctx, const std::wstring& shaderDir)
{
    this->device = device;
    this->context = ctx;

    //Compilazione shader
    if (!shaderRect.Load(device, shaderDir + L"vs.hlsl", shaderDir + L"ps.hlsl")) return false;
    if (!shaderCircle.Load(device, shaderDir + L"vs.hlsl", shaderDir + L"ps_circle.hlsl")) return false;

    //Definizione input layout (slide 42)
    D3D11_INPUT_ELEMENT_DESC ied[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },  //2 float per x e y
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    const void* bufferPointer = shaderRect.GetVSBlob()->GetBufferPointer();
    SIZE_T bufferSize = shaderRect.GetVSBlob()->GetBufferSize();

    HRESULT hr = device->CreateInputLayout(ied, 3, bufferPointer, bufferSize, &inputLayout);
    if (FAILED(hr)) return false;

    //Creazione buffer per accedere alla memoria video (slide 34)
    D3D11_BUFFER_DESC bd = {};
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;                             //Dynamic è meglio per la geometria dinamica del gioco
    bd.ByteWidth = sizeof(Vertex) * 6;                          //6 vertici per i rettangoli
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;                 //Serve per coerenza con D3D11_USAGE_DYNAMIC
    bd.MiscFlags = 0;
	hr = device->CreateBuffer(&bd, nullptr, &vertexBuffer);     //No InitData per coerenza con D3D11_USAGE_DYNAMIC
    if (FAILED(hr)) return false;

    //Configurazione per gestire la trasparenza (es HUD)
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;         //Peso del pixel nuovo
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;    //Peso del pixel già presente
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    device->CreateBlendState(&blendDesc, &blendState);

    float blendFactor[4] = { 0,0,0,0 };
    ctx->OMSetBlendState(blendState, blendFactor, 0xFFFFFFFF);

    return true;
}

//Helper, carica i 6 vertici nel vertex buffer ed effettua una draw call alla GPU
void Renderer2D::Draw(const Vertex v[6])
{
    //Sincronizzazione tra CPU e GPU per la lettura del buffer
    D3D11_MAPPED_SUBRESOURCE ms;
    context->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms); //[CPU] Aspetta e poi blocca il buffer
    memcpy(ms.pData, v, sizeof(Vertex) * 6);                        //[CPU] Copia i dati dei vertici nel buffer
    context->Unmap(vertexBuffer, 0);                                //[CPU] Libera il buffer

    //Preparazione chiamata draw (slide 44-45)
    context->IASetInputLayout(inputLayout); //Qui anzichè in Init per usare più shader con diversi input layout
    UINT stride = sizeof(Vertex), offset = 0;
    context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    //Disegna i 6 vertici (draw call alla GPU)
    context->Draw(6, 0);
}

void Renderer2D::DrawRect(float x, float y, float w, float h, Color c)
{
    //Conversione da pixel a NDC
    float left = ToNDC_X(x);
    float right = ToNDC_X(x + w);
    float top = ToNDC_Y(y);
    float bottom = ToNDC_Y(y + h);

    //Definizione dei 6 vertici
    Vertex v[6] = {
        { left,  top,    c.r, c.g, c.b, c.a, 0, 0 },
        { right, top,    c.r, c.g, c.b, c.a, 1, 0 },
        { right, bottom, c.r, c.g, c.b, c.a, 1, 1 },
        { left,  top,    c.r, c.g, c.b, c.a, 0, 0 },
        { right, bottom, c.r, c.g, c.b, c.a, 1, 1 },
        { left,  bottom, c.r, c.g, c.b, c.a, 0, 1 },
    };

    
	//Preparazione chiamata draw (slide 44-45)
	shaderRect.Bind(context); //Attivazione degli shader (slide 77)
	
    Draw(v);
}

void Renderer2D::DrawCircle(float x, float y, float r, Color c)
{
    //Conversione da pixel a NDC
    float left = ToNDC_X(x - r);
    float right = ToNDC_X(x + r);
    float top = ToNDC_Y(y - r);
    float bottom = ToNDC_Y(y + r);

    //Definizione dei 6 vertici
    Vertex v[6] = {
        { left,  top,    c.r, c.g, c.b, c.a, 0, 0 },
        { right, top,    c.r, c.g, c.b, c.a, 1, 0 },
        { right, bottom, c.r, c.g, c.b, c.a, 1, 1 },
        { left,  top,    c.r, c.g, c.b, c.a, 0, 0 },
        { right, bottom, c.r, c.g, c.b, c.a, 1, 1 },
        { left,  bottom, c.r, c.g, c.b, c.a, 0, 1 },
    };

    //Preparazione chiamata draw (slide 44-45)
    shaderCircle.Bind(context);                 //Attivazione degli shader (slide 77)

    Draw(v);
}

void Renderer2D::DrawLine(float xi, float yi, float xf, float yf, float thickness, Color c)
{
    float cx = std::abs(xf - xi);   //Componente orizzontale
    float cy = std::abs(yf - yi);   //Componente verticale
    
    if (cx <= 0.0f && cy <= 0.0f) return;  //Segmento degenere

    float x, y, w, h;

    //Disegna il segmento come un rettangolo, utilizzando la logica di DrawRect
    //Nota: sarà impreciso non essendo un rettangolo esattamente in diagonale, ma essendo piccoli segmenti non si nota troppo
    if (cy >= cx)   //Segmento prevalentemente verticale
    {
        float midX = (xi + xf) * 0.5f;
        x = midX - thickness * 0.5f;
        y = std::min(yi, yf);
        w = thickness;
        h = cy;

        /*
        *   xi
      -  ┌──┼──┐ <- y = min(yi,yf)
      |  │  │  │
      |  │  │  │
 cy = h  │  │<-│--- x = ((xi + xf) / 2) - w/2
      |  │  │  │  
      |  │  │  │
      -  └──┼──┘
            xf
         |--w--| == thickness
        */
    }
    else
    {
        //Segmento prevalentemente orizzontale
        float midY = (yi + yf) * 0.5f;
        x = std::min(xi, xf);
        y = midY - thickness * 0.5f;
        w = cx;
        h = thickness;
    }

    DrawRect(x, y, w, h, c);
}

void Renderer2D::Shutdown()
{
    if (!device) return;    //Per evitare che venga chiamato più volte (nel distruttore e in Game::Shutdown)
	//Rilascia risorse
    if (blendState) { blendState->Release();   blendState = nullptr; }
    if (vertexBuffer) { vertexBuffer->Release(); vertexBuffer = nullptr; }
    if (inputLayout) { inputLayout->Release();  inputLayout = nullptr; }
    shaderRect.Release();
    shaderCircle.Release();
}