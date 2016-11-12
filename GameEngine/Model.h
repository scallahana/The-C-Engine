//Filename: Model.h
////////////////////
#ifndef _MODELCLASS_H_
#define	_MODELCLASS_H_

//Includes
#include <D3D11.h>
#include <D3DX10math.h>


//Classname: ModelClass
class ModelClass
{
private:
	struct VertexType
	{
		D3DXVECTOR3 position;
		D3DXVECTOR4 color;
	};
public:
	ModelClass();
	ModelClass(const ModelClass&);
	~ModelClass();

	bool Initialize(ID3D11Device*);
	void ShutDown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutDownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	ID3D11Buffer *m_VertexBuffer, *m_IndexBuffer;
	int m_VertexCount, m_IndexCount;
};
#endif // _MODELCLASS_H_
