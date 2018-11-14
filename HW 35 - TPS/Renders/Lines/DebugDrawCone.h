#pragma once

#include "Line.h"

class DebugDrawCone : public Line
{
public:
	DebugDrawCone(float& height, float angle = 30.0f * D3DX_PI / 180.0f,
		int sliceCount = 36);
	~DebugDrawCone();

	void Render();
	void PostRender();

	void Get(float* pRadius, float* pHeight,
		float* pAngle) {
		pRadius = &radius;
		pHeight = &height;
		pAngle = &angle;
	}

	void Set(float& height, float angle);
	void Set(float& height, float& angle, D3DXCOLOR& color);

	void CreateVertex();
	void UpdateBuffer();

private:
	float angle;
	float radius;
	float height;
	int sliceCount;

	UINT lineCount;
	D3DXVECTOR3* lines;
};