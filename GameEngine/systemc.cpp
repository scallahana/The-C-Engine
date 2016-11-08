//Filename systemc.cpp
////////////////////////

#include "systemc.h"

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}


void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (full_screen)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	//Init the width and height of the screen to 0.
	screenWidth = 0;
	screenHeight = 0;

	//Init the windows API
	InitializeWindows(screenWidth, screenHeight);

	//Create input obj
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	//Init the input obj
	m_Input->Initialize();

	//Create graphics Obj
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	return true;

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		//Check if window destroyed
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	//Check if window is closing
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	//All otheres go to default message handler
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		//Check if key has been pressed.
	case WM_KEYDOWN:
	{
		//if key has been pressed send it to the input obj so it records state
		m_Input->KeyDown((unsigned int)wparam);
		return 0;
	}
	//check if key has been released
	case WM_KEYUP:
	{
		//if key has been released send it to the input obj so it can unset state
		m_Input->KeyUp((unsigned int)wparam);
		return 0;

	}
		//Any other message send to the default msg handler as our app wont make use of them.
	default:
	{
		return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	//Get external pointer to obj
	ApplicationHandle = this;

	//Get instance of this app
	m_hinstance = GetModuleHandle(NULL);


	//give application name;
	m_applicationName = L"The C Engine";

	//setup windows class w defaults
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);


	//Register Winders Class
	RegisterClassEx(&wc);


	//Determine res of clients desktop
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	//setup screen settings depending on whether its fullscreen or not.
	if (full_screen)
	{
		//if full screen set screen to maximum size of the users Desktop (32bit)
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		//change display to fullscreen
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		//set position of window left corner top
		posX = posY = 0;
	}
	else {
		//if windowed set it to 800x600 res
		screenWidth = 800;
		screenHeight = 600;

		//Place Window middle of screen
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	}

	//Create widnow with screen settings and get handle to it
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

}
void SystemClass::Shutdown()
{
	//Release graphics ALL OF THEM
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	//Release input 
	if (m_Input)
	{
		delete m_Input;
		m_Input = 0;

	}

	ShutdownWindows();

	return;
}

bool SystemClass::Frame()
{
	bool result;


	//Check if the user hit escape to exit 
	if (m_Input->isKeyDown(VK_ESCAPE))
	{
		return false;
	}

	//Do frame procressing of graphics
	result = m_Graphics->Frame();
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	//Init the message structure.
	ZeroMemory(&msg, sizeof(MSG));


	//Loop until quit msg from user.
	done = false;
	while (!done)
	{
		//Handle  windows message
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		//if windows signals end then exit out
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			//Otherwise do	frame proccessing.
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}

	return;
}

//copy construct
SystemClass::SystemClass(const SystemClass& other)
{

}

//Dstr
SystemClass::~SystemClass()
{

}