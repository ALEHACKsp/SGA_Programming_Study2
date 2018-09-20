#pragma once

#include "Line.h"

class DebugDrawCone2 : public Line
{
public:
	DebugDrawCone2(float& radius, float& height,
		int sliceCount = 36);
	~DebugDrawCone2();

	void Render();
	void PostRender();

	void Get(float* pRadius, float* pHeight,
		D3DXVECTOR3* pDir) {
		pRadius = &radius;
		pHeight = &height;
		pDir = &dir;
	}

	void Set(float& radius, float& height);
	void Set(float& radius, float& height, D3DXCOLOR& color);

	void CreateVertex();
	void UpdateBuffer();

private:
	float radius;
	float height;
	int sliceCount;

	D3DXVECTOR3 dir;

	UINT lineCount;
	D3DXVECTOR3* lines;
};