//Filename: Model.cpp
//////////////////////
#include "Model.h"

ModelClass::ModelClass()
{
	m_VertexBuffer = 0;
	m_IndexBuffer = 0;
	m_texture = 0;
}

ModelClass::ModelClass(const ModelClass& other)
{

}

ModelClass::~ModelClass()
{

}

bool ModelClass::Initialize(ID3D11Device* device, WCHAR* TexturefileName)
{
	bool result;


	//Init the vertex  and index bufferthat hold geomtry
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	//Load tex for this model
	result = LoadTexture(device, TexturefileName);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ShutDown()
{

	//release texture
	ReleaseTexture();
	//Release vertex and index buffers
	ShutDownBuffers();

	return;
}
void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	//set vertex buffer stride and offset
	stride = sizeof(VertexType);
	offset = 0;

	//Set vertex buffer to active in input assembler
	deviceContext->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	//set index buffer to active in input assembler
	deviceContext->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set primitive type that should be rendered from vertex buffer
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ModelClass::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;
	
	//Create Texture Object;
	m_texture = new TextureClass;
	if (!m_texture)
	{
		return false;
	}

	//init the texture obj.
	result = m_texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void ModelClass::ReleaseTexture()
{
	//Release texture obj
	if (m_texture)
	{
		m_texture->ShutDown();
		delete m_texture;
		m_texture = 0;
	}

	return;
}
ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_texture->GetTexture();
}
void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	//Put vertex and index buffers on graphics pipeline
	RenderBuffers(deviceContext);

	return;
}

int ModelClass::GetIndexCount()
{
	return m_IndexCount;
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	HRESULT result;

	//set number of verts in vertex array.
	m_VertexCount = 3;

	//Set number of indices in index array
	m_IndexCount = 3;

	//Create Vertex array
	vertices = new VertexType[m_VertexCount];
	if (!vertices)
	{
		return false;
	}

	//Create index array
	indices = new unsigned long[m_IndexCount];
	if (!indices)
	{
		return false;
	}


	//Load vertex array data
	vertices[0].position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f); //BTM left
	vertices[0].texture = D3DXVECTOR2(0.0f, 1.0f);

	vertices[1].position = D3DXVECTOR3(0.0f, 1.0f, 0.0f); //TP Mid
	vertices[1].texture = D3DXVECTOR2(0.5f, 0.0f);

	vertices[2].position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);  //BTM Right
	vertices[2].texture = D3DXVECTOR2(1.0f, 1.0f);
	
	//Give data to index Array
	indices[0] = 0; //BTM left
	indices[1] = 1; //TP Mid
	indices[2] = 2; //BTM Right

	//Setup desc of vertex BUffer
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) *m_VertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//Give sub-resource structure pointer  tovertex data
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;


	//now create vertex bufferr
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_VertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Setup desc of index buffer
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) *m_IndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//Give sub-resource structure pointer to index data
	indexData.pSysMem = indices;


	//Create index buffer
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_IndexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Release the arrays
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
	
	return true;
}

void ModelClass::ShutDownBuffers()
{
	//Release index Buffer
	if (m_IndexBuffer)
	{
		m_IndexBuffer->Release();
		m_IndexBuffer = 0;
	}

	//Release vertex buffer
	if (m_VertexBuffer)
	{
		m_VertexBuffer->Release();
		m_VertexBuffer = 0;
	}

	return;
}