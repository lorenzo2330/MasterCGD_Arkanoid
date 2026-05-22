#pragma comment(lib, "d3d11.lib")       
#pragma comment(lib, "d3dcompiler.lib") 
#pragma comment(lib, "dxgi.lib")        
#include "shader.h"
#include <d3d11.h>
#include <d3dcommon.h>
#include <d3dcompiler.h>
#include <string>
#include <Windows.h>



bool Shader::Load(ID3D11Device* device, const std::wstring& vsPath, const std::wstring& psPath)
{
    ID3DBlob* errorBlob = nullptr;

	//Compilazione vertex shader (slide 76)
    HRESULT hr = D3DCompileFromFile(            //D3DCompileFromFile perchè D3DX11CompileFromFile è deprecato
        vsPath.c_str(), 
        nullptr, 
        nullptr,
        "main", 
        "vs_5_0",                               //5.0 perchè è il nuovo standard
        D3DCOMPILE_ENABLE_STRICTNESS, 
        0,
        &vsBlob, 
        &errorBlob
    );
    
    if (FAILED(hr)) {
        if (errorBlob) { OutputDebugStringA((char*)errorBlob->GetBufferPointer()); errorBlob->Release(); }
        return false;
    }

	//Creazione vertex shader (slide 76)
    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vs);
    if (FAILED(hr)) return false;

	//Compilazione pixel shader (slide 76)
    ID3DBlob* psBlob = nullptr;
    hr = D3DCompileFromFile(
        psPath.c_str(), 
        nullptr, 
        nullptr,
        "main", 
        "ps_5_0",
        D3DCOMPILE_ENABLE_STRICTNESS, 
        0,
        &psBlob, 
        &errorBlob
    );

    if (FAILED(hr)) { if (errorBlob) { OutputDebugStringA((char*)errorBlob->GetBufferPointer()); errorBlob->Release(); } return false; }

    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &ps);
    psBlob->Release();
    if (FAILED(hr)) return false;

    return true;
}

void Shader::Bind(ID3D11DeviceContext* ctx) const
{
	//Slide 77
    //Attivazione degli shader
    ctx->VSSetShader(vs, nullptr, 0);
    ctx->PSSetShader(ps, nullptr, 0);
}

void Shader::Release()
{
	//Rilascio risorse
    if (vs) { vs->Release(); vs = nullptr; }
    if (ps) { ps->Release(); ps = nullptr; }
    if (vsBlob) { vsBlob->Release(); vsBlob = nullptr; }
}