//filename: Texture.cpp
////////////////////////
#include "Texture.h"

TextureClass::TextureClass()
{
	m_Texture = 0;
}

TextureClass::TextureClass(const TextureClass& other)
{

}

TextureClass::~TextureClass()
{

}

bool TextureClass::Initialize(ID3D11Device* device, WCHAR* fileName)
{
	HRESULT result;

	//Load texture
	result = D3DX11CreateShaderResourceViewFromFile(device, fileName, NULL, NULL, &m_Texture, NULL);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureClass::ShutDown()
{
	//Release texture
	if (m_Texture)
	{
		m_Texture->Release();
		m_Texture = 0;
	}

	return;
}

ID3D11ShaderResourceView* TextureClass::GetTexture()
{
	return m_Texture;
}