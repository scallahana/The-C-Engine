////File name: main.cpp
/////////////////////////

//includes
#include "systemc.h"

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevinstance, PSTR pScmdline, int iCmdshow)
{
	SystemClass* System;
	bool result;

	//Create system obj
	System = new SystemClass;
	if (!System)
	{
		return 0;
	}

	//Init and run System Obj
	result = System->Initialize();
	if (result)
	{
		System->Run();
	}

	//Shut down adn release the System obj
	
	System->Shutdown();
	delete System;
	
	System = 0;

	return 0;

}