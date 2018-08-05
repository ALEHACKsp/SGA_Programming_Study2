#pragma once

#include "Camera.h"

#define USINGVECTOR 0

class Orbit : public Camera
{
public:


#if USINGVECTOR
	Orbit(D3DXVECTOR3 targetPos = D3DXVECTOR3(0,0,0),
		float moveSpeed = 20.0f, float rotationSpeed = 2.5f);


	void SetTargetPos(D3DXVECTOR3* vec) {
		targetPos = *vec;
	}
#else
	Orbit(float moveSpeed = 20.0f, float rotationSpeed = 2.5f);

	void SetTargetMat(D3DXMATRIX* mat) {
		matTarget = mat;
	}
	void UpdateMatrix();
#endif
	~Orbit();

	void Update();

private:
	float moveSpeed;
	float rotationSpeed;

#if USINGVECTOR
	D3DXVECTOR3 targetPos;
#else
	D3DXMATRIX * matTarget;

	D3DXMATRIX mat;
	D3DXVECTOR3 pos;
#endif
};