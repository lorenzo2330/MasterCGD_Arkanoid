#pragma once
#include <d3d11.h>
#include <d3dcommon.h>
#include <string>

class Shader
{
public:
    Shader() = default;
    ~Shader() { Release(); }

    //Rule of five: se definisco distruttore, devo definire anche copy e move (FC++, lezione 7)
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader&) = delete;
	Shader(Shader&&) = delete;
	Shader& operator=(Shader&&) = delete;

    bool Load(ID3D11Device* device, const std::wstring& vsPath, const std::wstring& psPath);    //Compila gli shader
	void Bind(ID3D11DeviceContext* ctx) const;                                                  //Attiva gli shader
	ID3DBlob* GetVSBlob() const { return vsBlob; }                                              //Per l'InputLayout

	void Release();                                                                             //Rilascia risorse

private:
	ID3D11VertexShader* vs = nullptr;   //Vertex shader compilato
    ID3D11PixelShader* ps = nullptr;    //Pixel shader compilato
    ID3DBlob* vsBlob = nullptr;         //Per l'InputLayout
};