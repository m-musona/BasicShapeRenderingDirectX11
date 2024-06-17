#include "Shader.h"

#include <d3dcompiler.h>
#include <iostream>

Shader::Shader(const char* vertexShaderSource, const char* pixelShaderSource, ID3D11Device* dev)
{
    // Create VS constant buffer
    D3D11_BUFFER_DESC vsCBDesc;
    ZeroMemory(&vsCBDesc, sizeof(vsCBDesc));
    vsCBDesc.Usage = D3D11_USAGE_DYNAMIC;
    vsCBDesc.ByteWidth = sizeof(VSConstantBuffer);
    vsCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    vsCBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    dev->CreateBuffer(&vsCBDesc, nullptr, &mVSConstantBuffer);

    // Create PS constant buffer
    D3D11_BUFFER_DESC psCBDesc;
    ZeroMemory(&psCBDesc, sizeof(psCBDesc));
    psCBDesc.Usage = D3D11_USAGE_DYNAMIC;
    psCBDesc.ByteWidth = sizeof(PSConstantBuffer);
    psCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    psCBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    dev->CreateBuffer(&psCBDesc, nullptr, &mPSConstantBuffer);

    Compile(vertexShaderSource, pixelShaderSource);

    // Create shaders
    dev->CreateVertexShader(mVSBlob->GetBufferPointer(), mVSBlob->GetBufferSize(), nullptr, &mVS);
    dev->CreatePixelShader(mPSBlob->GetBufferPointer(), mPSBlob->GetBufferSize(), nullptr, &mPS);
}

Shader::~Shader()
{
    mPSConstantBuffer->Release();
    mVSConstantBuffer->Release();
}

void Shader::Compile(const char* vertexShaderSource, const char* pixelShaderSource)
{
    HRESULT hr = D3DCompile(vertexShaderSource, strlen(vertexShaderSource), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, &mVSBlob, &mErrorBlob);
    if (FAILED(hr)) {
        if (mErrorBlob) {
            std::cerr << "Vertex Shader Error: " << (char*)mErrorBlob->GetBufferPointer() << std::endl;
            mErrorBlob->Release();
        }
        if (mVSBlob) mVSBlob->Release();
        return;
    }

    hr = D3DCompile(pixelShaderSource, strlen(pixelShaderSource), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, &mPSBlob, &mErrorBlob);
    if (FAILED(hr)) {
        if (mErrorBlob) {
            std::cerr << "Pixel Shader Error: " << (char*)mErrorBlob->GetBufferPointer() << std::endl;
            mErrorBlob->Release();
        }
        if (mPSBlob) mPSBlob->Release();
        return;
    }
}
