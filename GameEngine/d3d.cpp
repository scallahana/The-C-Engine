//Filename: d3d.cpp
///////////////////

#include "d3d.h"
#include <iostream>
#include <fstream>
D3DClass::D3DClass()
{
	m_SwapChain = 0;
	m_Device = 0;
	m_DeviceContext = 0;
	m_RenderTarget_View = 0;
	m_DepthStencilBuffer = 0;
	m_DepthStencilView = 0;
	m_DepthStencilState = 0;
	m_RasterState = 0;
}

D3DClass::D3DClass(const D3DClass& other)
{

}

D3DClass::~D3DClass()
{

}

bool D3DClass::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear)
{
	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;

	unsigned int numModes, i, numerator, denominator, stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterizerDesc;
	D3D11_VIEWPORT viewport;
	float FOV, screenAspect;

	//store vsync settings
	m_Vsync_Enabled = vsync;

	//Create DX graphics interface factory
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	//Use factory to create adapter for video card
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	//Enum the primary moniter
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	//Get number of modes
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	//Actually assign a value to a variable
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	//Create a list to hold all display modes that matches screenWidth and height when 
	//found store it to the numerator and denominator of the refresh for moniter.
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	//Get video card description
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}


	//store VRAM in megabytes will write to file in the future. For fun reasons!
	m_VideoMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	//Convert name of video card  to char array and store it
	error = wcstombs_s(&stringLength, m_VideoCardDesc, 128, adapterDesc.Description,
		128);
	if (error != 0)
	{
		return false;
	}

	//release the display mode list
	delete[] displayModeList;
	displayModeList = 0;

	//Release adpater output
	adapterOutput->Release();
	adapterOutput = 0;

	//release the kraken(Adapter)
	adapter->Release();
	adapter = 0;

	//Release the factory
	factory->Release();
	factory = 0;

	//Will finish the rest of this extremely long method tomorrow 
	//Or the day after one of the two.

	//Init swap chain
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	//set to single back buffer
	swapChainDesc.BufferCount = 1;

	//swt width and height
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;


	//Set reg to 32 bit. for back buffer
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	//set refresh rate to back buffer;
	if (m_Vsync_Enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	} else {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	//Set usage of back buffer
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	//set handle for the winder to render to .
	swapChainDesc.OutputWindow = hwnd;

	//Turn MS off
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	//set to fullscreen or naw
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	} else {

		swapChainDesc.Windowed = true;

	}

	//Set scan line ordering  and scailing to not specified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;


	//discard back buffer contents after processing.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	//dont set advanced flags
	swapChainDesc.Flags = 0;

	//feature level;
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	//create swap chain
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_SwapChain, &m_Device, NULL, &m_DeviceContext);
	if (FAILED(result))
	{
		return false;
	}

	//Get pointer to back buffer
	result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	//Release pointer
	backBufferPtr->Release();
	backBufferPtr = 0;

	//Init depth buffer desc
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));


	//Setup desc of depth buffer
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	//create texture for depth buffer
	result = m_Device->CreateTexture2D(&depthBufferDesc, NULL, &m_DepthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}

	//Init the description of stencil state
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	//Setup description for stencil state
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	//Stencil ops if pixel is front facing;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//stencil ops if pixel is back facing
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//create depth stencil state
	result = m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	//set depth stencil state
	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);

	//init the stencil view
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	//Setup stencil view description
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	//create stencil view
	result = m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, &m_DepthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	//bind render target view and depth stencil buffer to output pipeline
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTarget_View, m_DepthStencilView);

	//Setup rasterizer desc
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	//create rasterizer state from desc
	result = m_Device->CreateRasterizerState(&rasterizerDesc, &m_RasterState);
	if (FAILED(result))
	{
		return false;
	}

	//set rasterizer state
	m_DeviceContext->RSSetState(m_RasterState);

	//Setup viewport
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	//Create viewport
	m_DeviceContext->RSSetViewports(1, &viewport);


	//Setup projection matrix.
	FOV = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	//Create matrix for rendering
	D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix, FOV, screenAspect, screenNear, screenDepth);

	//INIT world matrix
	D3DXMatrixIdentity(&m_WorldMatrix);

	 // Create an ortho projection  for 2D rendering.
	D3DXMatrixOrthoLH(&m_OrthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3DClass::ShutDown()
{
	//Before closing set to window mode. or it will throw and exception
	if (m_SwapChain)
	{
		m_SwapChain->SetFullscreenState(false, NULL);
	}

	if (m_RasterState)
	{
		m_RasterState->Release();
		m_RasterState = 0;
	}

	
	if (m_DepthStencilView)
	{
		m_DepthStencilView->Release();
		m_DepthStencilView = 0;
	}

	
	if (m_DepthStencilState)
	{
		m_DepthStencilState->Release();
		m_DepthStencilState = 0;
	}
	
		

	if (m_DepthStencilBuffer)
	{
		m_DepthStencilBuffer->Release();
		m_DepthStencilBuffer = 0;
	}

	if (m_RenderTarget_View)
	{
		m_RenderTarget_View->Release();
		m_RenderTarget_View = 0;
	}

	if (m_DeviceContext)
	{
		m_DeviceContext->Release();
		m_DeviceContext = 0;
	}

	if (m_Device)
	{
		m_Device->Release();
		m_Device = 0;
	}

	if (m_SwapChain)
	{
		m_SwapChain->Release();
		m_SwapChain = 0;
	}

	return;
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	//Set color to clear buffer to.
	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	//Clear back buffer
	m_DeviceContext->ClearRenderTargetView(m_RenderTarget_View, color);

	//Clear depth buffer
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}

void D3DClass::EndScene()
{
	//Present the back buffer to the screen rendering is done.
	if (m_Vsync_Enabled)
	{
		//Lock to screen refresh rate I.E mine is 75Hz;
		m_SwapChain->Present(1, 0);
	}
	else
	{
		//Present asap.
		m_SwapChain->Present(0, 0);
	}

	return;
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_Device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_DeviceContext;
}

void D3DClass::GetProjectionMatrix(D3DXMATRIX& projectionMatrix)
{
	projectionMatrix = m_ProjectionMatrix;
	return;
}

void D3DClass::GetWorldMatrix(D3DXMATRIX& worldMatrix)
{
	worldMatrix = m_WorldMatrix;
	return;
}

void D3DClass::GetOrthoMatrix(D3DXMATRIX& orthoMatrix)
{
	orthoMatrix = m_OrthoMatrix;
	return;
}

void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{

	//print all of this to a file!
	 strcpy_s(cardName, 128, m_VideoCardDesc);
	memory = m_VideoMemory;
	return;
}