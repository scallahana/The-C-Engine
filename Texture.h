//Filename: Texture.h
#ifndef _TEXTURECLASS_H_
#define _TEXTURECLASS_H_

//includes
#include <d3d11.h>
#include <D3DX11tex.h>

//Classname: TextureClass
class TextureClass
{
public:
	TextureClass();
	TextureClass(const TextureClass&);
	~TextureClass();

	bool Initialize(ID3D11Device*, WCHAR*);
	void ShutDown();
	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11ShaderResourceView* m_Texture;
};
#endif // !_TEXTURECLASS_H_
