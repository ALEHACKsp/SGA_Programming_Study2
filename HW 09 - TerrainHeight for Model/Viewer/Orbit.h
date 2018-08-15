#pragma once

#include "Camera.h"

class Orbit : public Camera {
public:
	Orbit(float moveSpeed = 20.0f, float rotationSpeed = 2.5f);
	~Orbit();

	void Update();

	void SetTargetPos(D3DXVECTOR3* targetPos);
	void SetMoveValue(float moveValue) { this->moveValue = moveValue; }
private:
	float moveSpeed;
	float rotationSpeed;

	D3DXVECTOR3* targetPos;
	float moveValue;
};