//Filename: graphics.h
///////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

//includes
#include "d3d.h"
////

//Globals
//
//////////
const bool full_screen = false;
const bool Vsync_enabled = false;
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

	D3DClass* m_D3D;

};

#endif 