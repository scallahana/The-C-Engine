//Filename: d3d.cpp
///////////////////

#include "d3d.h"

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
	if(error != 0)
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

}