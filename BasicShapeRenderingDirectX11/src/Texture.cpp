#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <stdexcept>

Texture::Texture(std::string& texturePath, ID3D11Device* dev)
{
    ImageData imageData = LoadImageFromFile(texturePath);
    CreateTextureFromImageData(imageData, &mTexture, &mTextureView, dev);
}

Texture::~Texture()
{
    mTexture->Release();
    mTextureView->Release();
}

Texture::ImageData Texture::LoadImageFromFile(std::string& filename)
{
	int width, height, channels;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
	if (!data) {
		throw std::runtime_error("Failed to load image");
	}

	std::vector<unsigned char> imageData(data, data + width * height * 4);
	stbi_image_free(data);

	return { width, height, 4, std::move(imageData) };
}

void Texture::CreateTextureFromImageData(const ImageData& imageData, ID3D11Texture2D** texture, ID3D11ShaderResourceView** textureView, ID3D11Device* dev)
{
    D3D11_TEXTURE2D_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.Width = imageData.width;
    desc.Height = imageData.height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = imageData.data.data();
    initData.SysMemPitch = static_cast<UINT>(imageData.width * 4);
    initData.SysMemSlicePitch = 0;

    HRESULT hr = dev->CreateTexture2D(&desc, &initData, texture);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create texture");
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    ZeroMemory(&srvDesc, sizeof(srvDesc));
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    hr = dev->CreateShaderResourceView(*texture, &srvDesc, textureView);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create shader resource view");
    }
}
