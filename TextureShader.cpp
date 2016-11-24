//Filename: textureShader.cpp
/////////////////////////////
#include "TextureShader.h"
#include <fstream>

TextureShaderClass::TextureShaderClass()
{
	m_VertexShader = 0;
	m_PixelShader = 0;
	m_Layout = 0;
	m_MatrixBuffer = 0;
	m_SampleState = 0;
}

TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{

}

TextureShaderClass::~TextureShaderClass()
{

}
void TextureShaderClass::OutputShaderMsg(ID3D10Blob* errorMsg, HWND hwnd, WCHAR* shaderFileName)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream shaderErrorFile;

	//get pointer to the error text buffer
	compileErrors = (char*)(errorMsg->GetBufferPointer());

	bufferSize = errorMsg->GetBufferSize();

	shaderErrorFile.open("shader-error.txt");

	//write out error msg
	for (i = 0; i < bufferSize; i++)
	{
		shaderErrorFile << compileErrors[i];

	}

	shaderErrorFile.close();

	//Release error message
	errorMsg->Release();
	errorMsg = 0;

	//Pop up message on screen
	MessageBox(hwnd, L"Error compiling shader. Check shader-error.md for message", shaderFileName, MB_OK);

	return;
}
void TextureShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	deviceContext->IASetInputLayout(m_Layout);

	//Set the vertex and pixel shader
	deviceContext->VSSetShader(m_VertexShader, NULL, 0);
	deviceContext->PSSetShader(m_PixelShader, NULL, 0);

	//set sampler state in pixel shader
	deviceContext->PSSetSamplers(0, 1, &m_SampleState);

	//render triangle
	deviceContext->DrawIndexed(indexCount, 0, 0);
	return;
}
bool TextureShaderClass::SetShaderParams(ID3D11DeviceContext* deviceContext, D3DXMATRIX WorldMatrix, D3DXMATRIX ViewMatrix, D3DXMATRIX ProjectionMatrix,
	ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//transpose the matrices to prepare them for shader
	D3DXMatrixTranspose(&WorldMatrix, &WorldMatrix);
	D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix);
	D3DXMatrixTranspose(&ProjectionMatrix, &ProjectionMatrix);

	//lock the constant buffer so we can write to it
	//Letters maybe some Edger Allan Poe.
	result = deviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	//Get a pointer in the constant buffer
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//copy matrices to constant buffer
	dataPtr->world = WorldMatrix;
	dataPtr->view = ViewMatrix;
	dataPtr->projection = ProjectionMatrix;


	//unlock constant buffer
	deviceContext->Unmap(m_MatrixBuffer, 0);

	//setup the position of the constant buffer vertex shader
	bufferNumber = 0;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);

	//Set shader texture resource 
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;

}
bool TextureShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
	HRESULT result;
	ID3D10Blob* errorMsg;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC sampleDesc;


	//init pointers
	errorMsg = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//compile vertex shader code
	result = D3DX11CompileFromFile(vsFileName, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer, &errorMsg, NULL);
	if (FAILED(result))
	{
		//if shader failed to compile throw an error
		if (errorMsg)
		{
			OutputShaderMsg(errorMsg, hwnd, vsFileName);
		}
		else
		{
			MessageBox(hwnd, vsFileName, L"Missing Shader File.", MB_OK);
		}

		return false;
	}

	//Compile Pixel shader code.
	result = D3DX11CompileFromFile(psFileName, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer, &errorMsg, NULL);
	if (FAILED(result))
	{
		if (errorMsg)
		{
			OutputShaderMsg(errorMsg, hwnd, psFileName);
		}
		else
		{
			MessageBox(hwnd, psFileName, L"Missing Shader File.", MB_OK);
		}

		return false;
	}

	//Create the vertex shader from buffer
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_VertexShader);
	if (FAILED(result))
	{
		return false;
	}

	//Create pixel shader from buffer
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_PixelShader);
	if (FAILED(result))
	{
		return false;
	}


	//Create vertex input layout.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//get count 
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//create vertex input layer
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_Layout);
	if (FAILED(result))
	{
		return false;
	}

	//release the vertex shader and pixel shader since they are no longer needed
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//setup description of the dyanmic matrix constant buffer
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//Create constant buffer pointer so we can access vertex shader constant buffer
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	
	//create texture sampler state desc
	sampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleDesc.MipLODBias = 0.0f;
	sampleDesc.MaxAnisotropy = 1;
	sampleDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampleDesc.BorderColor[0] = 0;
	sampleDesc.BorderColor[1] = 0;
	sampleDesc.BorderColor[2] = 0;
	sampleDesc.BorderColor[3] = 0;
	sampleDesc.MinLOD = 0;
	sampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

	//create texture sampler state
	result = device->CreateSamplerState(&sampleDesc, &m_SampleState);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}
bool TextureShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//Init the vertex and pixel shaders
	result = InitializeShader(device, hwnd, L"Shaders/Texture.vs", L"Shaders/Texture.ps");
	if (!result)
	{
		return false;
	}

	return true;
}

bool TextureShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX WorldMatrix, D3DXMATRIX ViewMatrix, D3DXMATRIX ProjectionMatrix,
	ID3D11ShaderResourceView* Texture)
{
	bool result;

	//Set shader params we will use for rendering
	result = SetShaderParams(deviceContext, WorldMatrix, ViewMatrix, ProjectionMatrix, Texture);
	if (!result)
	{
		return false;
	}

	RenderShader(deviceContext, indexCount);

	return true;
}

void TextureShaderClass::ShutDownShader()
{
	if (m_SampleState)
	{
		m_SampleState->Release();
		m_SampleState = 0;
	}

	if (m_MatrixBuffer)
	{
		m_MatrixBuffer->Release();
		m_MatrixBuffer = 0;
	}

	//Releases layout
	if (m_Layout)
	{
		m_Layout->Release();
		m_Layout = 0;
	}
	
	if (m_PixelShader)
	{
		m_PixelShader->Release();
		m_PixelShader = 0;
	}

	if (m_VertexShader)
	{
		m_VertexShader->Release();
		m_VertexShader = 0;
	}

	return;
}
void TextureShaderClass::ShutDown()
{
	//Shutdown vertex and pixel shader
	ShutDownShader();

	return;
}