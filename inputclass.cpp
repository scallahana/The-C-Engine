//Filename: inputclass.cpp
////////////////////////////

#include "input.h"

InputClass::InputClass()
{

}

InputClass::InputClass(const InputClass& other)
{

}

InputClass::~InputClass()
{

}

void InputClass::Initialize()
{
	int i;

	//Init all keys being released and not pressed
	for (i = 0; i < 256; i++)
	{
		m_keys[i] = false;
	}

	return;
}

void InputClass::KeyDown(unsigned int input)
{
	//if key pressed save it to key array
	m_keys[input] = true;
	
	return;
}

void InputClass::KeyUp(unsigned int input)
{
	//if key released clear it and save it to key array 
	m_keys[input] = false;
	
	return;

}

bool InputClass::isKeyDown(unsigned int key)
{
	//Return the key's state released or pressed
	return m_keys[key];
}