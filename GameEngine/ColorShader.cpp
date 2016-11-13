//Filename: ColorShader.cpp
////////////////////////////
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
	//Will finish rest of function later.
}
bool ColorShaderClass::Render(ID3D11DeviceContext* deviceContext, int indexCount, D3DXMATRIX WorldMatrix, D3DXMATRIX ViewMatrix, D3DXMATRIX ProjectionMatrix)
{
	bool result;

	//Set shader params that it will use for rendering
	if (!result)
	{
		return false;
	}

	//Now render prepared buffers
	RenderShader(deviceContext, indexCount);

	return true;
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
	result = InitializeShader(device, hwnd, L"/Shaders/Color.vs", L"/Shaders/Color.ps");
	if (!result)
	{
		return false;
	}

	return true;
}