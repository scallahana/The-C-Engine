//Filename: graphicsclass.cpp
/////////////////////////////

#include "graphics.h"

GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;
	
	//Create D3D obj
	m_D3D = new D3DClass;
	
	if (!m_D3D)
	{
		return false;
	}

	//Init the D3D obj
	result = m_D3D->Initialize(screenWidth, screenHeight, Vsync_enabled, hwnd, full_screen, Screen_depth, Screen_near);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	if (m_D3D)
	{
		m_D3D->Shutdown();
		
		delete m_D3D;
		
		m_D3D = 0;

	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	//Render scene.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::Render()
{

	//Clear buffers to begin scene
	m_D3D->BeginScene(0.5f, 0.5f, 0.5f, 1.0f);

	//Present rendered scene;
	m_D3D->EndScene();

	return true;
}