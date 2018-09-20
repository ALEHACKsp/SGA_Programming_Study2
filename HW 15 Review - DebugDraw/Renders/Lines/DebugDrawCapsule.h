#pragma once

#include "Line.h"

class DebugDrawCapsule : public Line
{
public:
	DebugDrawCapsule(float& radius, float height, int stackCount = 20, int sliceCount = 20);
	~DebugDrawCapsule();

	void Render();
	void PostRender();

	void Get(float* pRadius, float* pHeight) {
		*pRadius = radius;
		*pHeight = height;
	}

	void Set(float& radius, float& height);
	void Set(float& radius, float& height, D3DXCOLOR& color);

	void CreateVertex();
	void UpdateBuffer();

private:
	float radius;
	float height;
	int stackCount;
	int sliceCount;

	UINT lineCount;
	D3DXVECTOR3* lines;
};