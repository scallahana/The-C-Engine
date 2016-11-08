//Filename: graphics.h
///////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

//includes
#include <windows.h>
////

//Globals
//
//////////
const bool full_screen = true;
const bool Vsync_enabled = true;
const float Screen_depth = 1000.0f;
const float Screen_near = 0.1f;


//Class: Graphix class
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();

	bool Frame();

private:
	bool Render();

private:

};

#endif 