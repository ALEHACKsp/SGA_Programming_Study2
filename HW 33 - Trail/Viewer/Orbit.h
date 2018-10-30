#pragma once

#include "Camera.h"

class Orbit : public Camera {
public:
	~Orbit();

	void Update();
	Orbit(float moveSpeed = 20.0f, float rotationSpeed = 2.5f);

	void SetTarget(D3DXVECTOR3& targetPos) override;

	void SetOrbitDist(float dist) { orbitDist = dist; }
	float GetOrbitDist() { return orbitDist; }
private:
	float moveSpeed;
	float rotationSpeed;

	float orbitDist;
	D3DXVECTOR3 targetPos;
};