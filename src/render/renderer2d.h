#pragma once
#include "../colors.h"
#include "shader.h"
#include <d3d11.h>
#include <string>

class Renderer2D
{
public:
    Renderer2D() = default;
    ~Renderer2D() { Shutdown(); }

    //Rule of five: se definisco distruttore, devo definire anche copy e move (FC++, lezione 7)
    Renderer2D(const Renderer2D&) = delete;
    Renderer2D& operator=(const Renderer2D&) = delete;
    Renderer2D(Renderer2D&&) = delete;
    Renderer2D& operator=(Renderer2D&&) = delete;

    //Setup
    bool Init(ID3D11Device* device, ID3D11DeviceContext* ctx, const std::wstring& shaderDir);
    void Shutdown();

    //Permette di disegnare i rettangoli, prende posizione, dimensione e colore
    void DrawRect(float x, float y, float w, float h, Color c);

    //Permette di disegnare i cerchi, prende posizione, dimensione e colore
    void DrawCircle(float x, float y, float r, Color c);

    //Permette di disegnare dei segmenti, prende due punti, spessore del segmento e colore
    void DrawLine(float xi, float yi, float xf, float yf, float thickness, Color c);

private:
    struct Vertex { float x, y, r, g, b, a, u, v; };  //Struttura del vertice (no z perchè inutile in 2d)

    //Helper, carica i 6 vertici nel vertex buffer ed effettua una draw call alla GPU
    void Draw(const Vertex v[6]);

    ID3D11Device* device = nullptr;
    ID3D11DeviceContext* context = nullptr;

    Shader shaderRect;
    Shader shaderCircle;
    ID3D11InputLayout* inputLayout = nullptr;
    ID3D11Buffer* vertexBuffer = nullptr;
    ID3D11BlendState* blendState = nullptr;

};