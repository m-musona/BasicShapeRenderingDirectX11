#pragma once
#include <d3d11.h>
#include <vector>

class VertexBuffer
{
public:
	VertexBuffer(std::vector<float> vertices, ID3D11Device* dev);
	~VertexBuffer();
	ID3D11Buffer* GetVertexBuffer() { return mVertexBuffer; }
private:
	ID3D11Buffer* mVertexBuffer;
};

class IndexBuffer
{
public:
	IndexBuffer(std::vector<unsigned int> indices, ID3D11Device* dev);
	~IndexBuffer();
	ID3D11Buffer* GetIndexBuffer() const { return mIndexBuffer; }
	size_t GetIndicesSize() const { return mIndicesSize; }
private:
	ID3D11Buffer* mIndexBuffer;
	size_t mIndicesSize;
};