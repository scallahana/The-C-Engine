//filename: Camera.h
//////////////////////
#ifndef _CAMERACLASS_H_
#define _CAMERACLASS_H_

//includes
///////////
#include <d3dx10math.h>


//Classname: CameraClass
//////////////////////////

class CameraClass
{
public:
	CameraClass();
	CameraClass(const CameraClass&);
	~CameraClass();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX&);

private:
	float m_posX, m_posY, m_posZ;
	float m_rotationX, m_rotationY, m_rotationZ;
	D3DXMATRIX m_ViewMatrix;
};
#endif // !_CAMERACLASS_H_