//Filename: graphicsclass.cpp
/////////////////////////////

#include "graphics.h"
GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_ColorShader = 0;
	m_Model = 0;
	m_TextureShader = 0;
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

	//Create camera obj
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	m_Camera->SetPosition(0.0f, 0.0f, -05.0f);

	//Create model obj 
	m_Model = new ModelClass;
	if (!m_Model)
	{
		return false;
	}

	result = m_Model->Initialize(m_D3D->GetDevice(), L"Textures/checkered.DDS");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the model object.", L"ERROR", MB_OK);
		return false;
	}

	//Create texture obj
	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}

	//Init  the texture shader obj
	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not intialize texture shader object", L"ERROR", MB_OK);
		return false;
	}

	////Create colorshader obj
	//m_ColorShader = new ColorShaderClass;
	//if (!m_ColorShader)
	//{
	//	return false;
	//}

	//result = m_ColorShader->Initialize(m_D3D->GetDevice(), hwnd);
	//if (!result)
	//{
	//	MessageBox(hwnd, L"Could not initialzie the colorShader object.", L"ERROR", MB_OK);
	//	return false;
	//}
	return true;
}


void GraphicsClass::Shutdown()
{
	//release texture obj
	if (m_TextureShader)
	{
		m_TextureShader->ShutDown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}
	/*if (m_ColorShader)
	{
		m_ColorShader->ShutDown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}
*/
	// Release the model object.
	if (m_Model)
	{
		m_Model->ShutDown();
		delete m_Model;
		m_Model = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	if (m_D3D)
	{
		m_D3D->ShutDown();
		
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
	D3DXMATRIX ViewMatrix, ProjectionMatrix, WorldMatrix;
	bool result;

	//Clear buffers 
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	//Generate view matrix based on camera's pos
	m_Camera->Render();

	//Get world projection and view matrix from camera and D3D objects
	m_Camera->GetViewMatrix(ViewMatrix);
	m_D3D->GetWorldMatrix(WorldMatrix);
	m_D3D->GetProjectionMatrix(ProjectionMatrix);

	//Put model vertex and index buffers on graphics pipleline
	m_Model->Render(m_D3D->GetDeviceContext());

	//render model using the texture shader
	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), WorldMatrix, ViewMatrix, ProjectionMatrix,
		m_Model->GetTexture());
	if (!result)
	{
		return false;

	}
	//render model using color shader
	/*result = m_ColorShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), WorldMatrix, ViewMatrix, ProjectionMatrix);
	if (!result)
	{
		return false;
	}*/

	//Present rendered scene;
	m_D3D->EndScene();

	return true;
}