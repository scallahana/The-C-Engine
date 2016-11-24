//Filename: ColorShader.cpp
////////////////////////////
//Does shader stuff.
#include "ColorShader.h"

ColorShaderClass::ColorShaderClass()
{

	m_VertexShader = 0;
	m_PixelShader = 0;
	m_Layout = 0;
	m_MatrixBuffer = 0;

}

ColorShaderClass::ColorShaderClass(const ColorShaderClass& other)
{

}

ColorShaderClass::~ColorShaderClass()
{

}

void ColorShaderClass::OutputShaderErrorMsg(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	std::ofstream shaderErrorFile;

	//Get pointer to error msg text buffer
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	//Get length of the message
	bufferSize = errorMessage->GetBufferSize();

	//Open a file to write  error message too
	shaderErrorFile.open("Shader-Errors.md");

	//Write  error message
	for (i = 0; i < bufferSize; i++)
	{
		shaderErrorFile << compileErrors[i];
	}

	shaderErrorFile.close();

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"Error Compiling Shader. Check Shader-Errors.md for message.", shaderFilename, MB_OK);
	return;
}
void ColorShaderClass::RenderShader(ID3D11DeviceContext* deviceContext, int indexCount)
{
	//Set vertex input layout
	deviceContext->IASetInputLayout(m_Layout);
	
	//Set the vertex and pixel shaders that will be used to render trig
	deviceContext->VSSetShader(m_VertexShader, NULL, 0);
	deviceContext->PSSetShader(m_PixelShader, NULL, 0);

	//render trig
	deviceContext->DrawIndexed(indexCount, 0, 0);

	return;
}
bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext* deviceContext, D3DXMATRIX WorldMatrix, D3DXMATRIX ViewMatrix, D3DXMATRIX ProjectionMatrix)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;

	//Transpose the matrices to prepare them for the shader
	D3DXMatrixTranspose(&WorldMatrix, &WorldMatrix);
	D3DXMatrixTranspose(&ViewMatrix, &ViewMatrix);
	D3DXMatrixTranspose(&ProjectionMatrix, &ProjectionMatrix);

	//lock the constant buffer so it can be written too.
	result = deviceContext->Map(m_MatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	dataPtr = (MatrixBufferType*)mappedResource.pData;

	//Copy matrices into the constant buffer
	dataPtr->World = WorldMatrix;
	dataPtr->View = ViewMatrix;
	dataPtr->Projection = ProjectionMatrix;

	//Unlock constant buffer
	deviceContext->Unmap(m_MatrixBuffer, 0);

	//Set pos of constant buffer in vertex shader
	bufferNumber = 0;
	
	//Set constant buffer in vertex shader with updated values
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_MatrixBuffer);

	return true;

}

bool ColorShaderClass::InitializeShader(ID3D11Device* device, HWND hwnd, WCHAR* vsFileName, WCHAR* psFileName)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;

	//Init the pointers this function will use to null
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	//Compile vertex shader
	result = D3DX11CompileFromFile(vsFileName, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &vertexShaderBuffer,
		&errorMessage, NULL);
	if (FAILED(result))
	{
		//If shader failed to compile should have written why to the error message
		if (errorMessage)
		{
			OutputShaderErrorMsg(errorMessage, hwnd, vsFileName);
		}
		else {
			MessageBox(hwnd, vsFileName, L"MISSING SHADER FILE", MB_OK);
		}

		return false;
	}

	//Compile PixelShader code
	result = D3DX11CompileFromFile(psFileName, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, &pixelShaderBuffer,
		&errorMessage, NULL);
	if (FAILED(result))
	{
		//if shader failed to compile should have written why to the error message
		if (errorMessage)
		{
			OutputShaderErrorMsg(errorMessage, hwnd, psFileName);
		}
		else {
			MessageBox(hwnd, psFileName, L"MISSING SHADER FILE", MB_OK);
		}

		return false;
	}

	//Create vertex shader from buffer
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

	//Now setup layout of the data that goes into the shader
	//This setup needs to match VertexType in ModelClass and shader
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	//Get count of elements in layout
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//create vertex input layout
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_Layout);
	if (FAILED(result))
	{
		return false;
	}

	//Release vertex and buffer shader since theyre no longer neeeded
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//setup desc of a matrix dynamic constant buffer 
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	//Create constant buffer pointer so we can access vertex shader from within this class
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_MatrixBuffer);
	if (FAILED(result))
	{
		return false;
	}

	return true;


}


bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX WorldMatrix, D3DXMATRIX ViewMatrix, D3DXMATRIX ProjectionMatrix)
{
	bool result;

	//Set shader params that it will use for rendering
	result = SetShaderParameters(deviceContext, WorldMatrix, ViewMatrix, ProjectionMatrix);
	if (!result)
	{
		return false;
	}

	//Now render prepared buffers
	RenderShader(deviceContext, indexCount);

	return true;
}
void ColorShaderClass::ShutDownShader()
{
	//release the matrix constant buffer
	if (m_MatrixBuffer)
	{
		m_MatrixBuffer->Release();
		m_MatrixBuffer = 0;
	}

	//release layout
	if (m_Layout)
	{
		m_Layout->Release();
		m_Layout = 0;
	}

	//release pixel shader
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
void ColorShaderClass::ShutDown()
{
	//Shutdown vertex and pixel shader
	ShutDownShader();

	return;
}

bool ColorShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
	bool result;

	//Init vertex and pixel shaders
	result = InitializeShader(device, hwnd, L"Shaders/Color.vs", L"Shaders/Color.ps");
	if (!result)
	{
		return false;
	}

	return true;
}