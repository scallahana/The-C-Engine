//Filename: Camera.cpp
///////////////////////
#include "Camera.h"

CameraClass::CameraClass()
{
	m_posX = 0.0f;
	m_posY = 0.0f;
	m_posZ = 0.0f;

	m_RotationX = 0.0f;
	m_RotationY = 0.0f;
	m_RotationZ = 0.0f;
}

CameraClass::CameraClass(const CameraClass& other)
{

}

CameraClass::~CameraClass()
{

}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_posX = x;
	m_posY = y;
	m_posZ = z;

	return;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_RotationX = x;
	m_RotationY = y;
	m_RotationZ = z;
	return;
}

D3DXVECTOR3 CameraClass::GetPosition()
{
	return D3DXVECTOR3(m_posX, m_posY, m_posZ);
}

D3DXVECTOR3 CameraClass::GetRotation()
{
	return D3DXVECTOR3(m_RotationX, m_RotationY, m_RotationZ);
}

void CameraClass::GetViewMatrix(D3DXMATRIX& ViewMatrix)
{
	ViewMatrix = m_ViewMatrix;
	return;
}
void CameraClass::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll; 
	D3DXMATRIX rotationMatrix;

	//Set up the vector that points upwards
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	//setup pos in the world
	position.x = m_posX;
	position.y = m_posY;
	position.z = m_posZ;

	//setup default camera lookat coords
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	//set yaw pitch and roll rotations in radians
	pitch = m_RotationX * 0.0174532925f;
	yaw = m_RotationY * 0.0174532925f;
	roll = m_RotationZ * 0.0174532925f;

	//create rotation matrix from yaw pitch and roll vals
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);
	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);

	//Translate the rotated camera position to the location of viewer
	lookAt = position + lookAt;

	D3DXMatrixLookAtLH(&m_ViewMatrix, &position, &lookAt, &up);

	return;
}