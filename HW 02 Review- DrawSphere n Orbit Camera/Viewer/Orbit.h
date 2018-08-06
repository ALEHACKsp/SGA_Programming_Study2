#pragma once

#include "Camera.h"

class Orbit : public Camera {
public:
	~Orbit();

	void Update();
#if CASE
	Orbit(float moveSpeed = 20.0f, float rotationSpeed = 2.5f);

	void View();

	void SetForcusPos(D3DXVECTOR3 pos) { forcusPos = pos; }
	D3DXVECTOR3 GetForcusPos() { return forcusPos; }

	void SetOrbitDist(float dist) { orbitDist = dist; }
	float GetOrbitDist() { return orbitDist; }
#else 
	Orbit(D3DXVECTOR3 targetPos = D3DXVECTOR3(0,0,0), 
		float moveSpeed = 20.0f, float rotationSpeed = 2.5f);

	void SetMoveValue(float moveValue) {
		this->moveValue = moveValue;
	}
#endif
private:
	float moveSpeed;
	float rotationSpeed;

#if CASE
	float orbitDist;
	D3DXVECTOR3 forcusPos;
#else 
	D3DXVECTOR3 targetPos;
	float moveValue;
#endif
};