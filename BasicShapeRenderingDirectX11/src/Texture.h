#pragma once
#include <d3d11.h>
#include <string>
#include <vector>
class Texture
{
public:
	struct ImageData {
		int width;
		int height;
		int channels;
		std::vector<unsigned char> data;
	};
	Texture(std::string& texturePath, ID3D11Device* dev);
	~Texture();
	ID3D11ShaderResourceView* GetTextureView() const { return mTextureView; }
private:
	ImageData LoadImageFromFile(std::string& filename);
	void CreateTextureFromImageData(const ImageData& imageData, ID3D11Texture2D** texture, ID3D11ShaderResourceView** textureView, ID3D11Device* dev);
private:
	ID3D11Texture2D* mTexture;
	ID3D11ShaderResourceView* mTextureView;
};