#pragma once

#include "Camera.h"

class SmoothFollow : public Camera
{
public:
	SmoothFollow(float distance = 10.0f, float height = 5.0f, float heightDamping = 2.0f, float rotationDamping = 1.0f);
	~SmoothFollow();

	void Update() override;
	void SetTarget(D3DXVECTOR3& targetPos, D3DXVECTOR3& targetRot, D3DXVECTOR3& targetUp) override;
	void PostRender() override;

	void SetHeight(float height) { this->height = height; }
	float GetHeight() { return height; }
	
	void SetDistance(float distance) { this->distance = distance; }
	float GetDistance() { return distance; }

private:
	D3DXVECTOR3 targetPos;
	D3DXVECTOR3 targetRot;
	D3DXVECTOR3 targetUp;

	float distance;
	float height;

	float heightDamping;
	float rotationDamping;
};