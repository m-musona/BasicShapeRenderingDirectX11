#include "Buffer.h"
#include <iostream>

VertexBuffer::VertexBuffer(std::vector<float> vertices, ID3D11Device* dev)
{
    // create the vertex buffer
    D3D11_BUFFER_DESC vertexBufferDesc = {};

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
    vertexBufferDesc.ByteWidth = vertices.size() * sizeof(float);             // size is the VERTEX struct * 3
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
    vertexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertexData = {};
    vertexData.pSysMem = vertices.data();

    HRESULT hr = dev->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);
    if (FAILED(hr))
    {
        std::cerr << "Failed create vertex buffer" << std::endl;
        exit(-1);
    }
}
VertexBuffer::~VertexBuffer()
{
    mVertexBuffer->Release();
}

IndexBuffer::IndexBuffer(std::vector<unsigned int> indices, ID3D11Device* dev)
{
    D3D11_BUFFER_DESC indexBufferDesc = {};
    indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
    indexBufferDesc.ByteWidth = sizeof(unsigned int) * indices.size();
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = 0;
    indexBufferDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA indexData = {};
    indexData.pSysMem = indices.data();

    HRESULT hr = dev->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer);
    if (FAILED(hr))
    {
        std::cerr << "Failed create index buffer" << std::endl;
        exit(-1);
    }

    mIndicesSize = indices.size();
}

IndexBuffer::~IndexBuffer()
{
    mIndexBuffer->Release();
}
