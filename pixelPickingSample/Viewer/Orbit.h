#pragma once
#include "Camera.h"

class Orbit : public Camera
{
public:
	Orbit(D3DXVECTOR3* focusPoint, float orbitRadius = 5.0f, float moveSpeed = 20.0f, float rotationSpeed = 2.5f);
	~Orbit();

	void Update();

	void SetFocusPoint(D3DXVECTOR3& focusPoint);

private:
	D3DXVECTOR3* focusPoint;
	D3DXVECTOR3 setFocusPoint;

	float moveSpeed;
	float rotationSpeed;
	
	D3DXVECTOR2 rotate;
	
	float orbitRadius;
};