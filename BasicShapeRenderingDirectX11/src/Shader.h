#pragma once
#include <d3d11.h>
#include <glm/glm.hpp>
#include <DirectXMath.h>

class Shader
{
public:
	struct PSConstantBuffer {
		glm::vec4 color;
		//float padding[3];  // Padding to ensure the constant buffer is a multiple of 16 bytes
	};
	struct VSConstantBuffer {
		DirectX::XMMATRIX model;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		//float padding[3];  // Padding to ensure the constant buffer is a multiple of 16 bytes
	};
	Shader(const char* vertexShaderSource, const char* pixelShaderSource, ID3D11Device* dev);
	~Shader();
	void Compile(const char* vertexShaderSource, const char* pixelShaderSource);

	ID3D11VertexShader* GetVertexShader() const { return mVS; }
	ID3D11PixelShader* GetPixelShader() const { return mPS; }

	ID3DBlob* GetVSBlob() const { return mVSBlob; }
	ID3DBlob* GetPSBlob() const { return mPSBlob; }
	ID3DBlob* GetErrorBlob() const { return mErrorBlob; }
	ID3D11Buffer* GetPSConstantBuffer() const { return mPSConstantBuffer; }
	ID3D11Buffer* GetVSConstantBuffer() const { return mVSConstantBuffer; }

private:
	ID3DBlob* mVSBlob = nullptr;
	ID3DBlob* mPSBlob = nullptr;
	ID3DBlob* mErrorBlob = nullptr;

	ID3D11VertexShader* mVS;    // the vertex shader
	ID3D11PixelShader* mPS;     // the pixel shader

	ID3D11Buffer* mPSConstantBuffer;
	ID3D11Buffer* mVSConstantBuffer;
};