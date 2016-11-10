//Filename: d3d.h
//////////////////
#ifndef _D3DCLASS_H_
#define _D3DCLASS_H_

///Linking
////
////////
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")

//includes
//////////
#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <d3dx10math.h>

///Classname: D3DClass
class D3DClass
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, HWND, bool, float, float);
	void ShutDown();

	void BeginScene();
	void EndScene();

};
#endif // !_D3DCLASS_H_
                   
