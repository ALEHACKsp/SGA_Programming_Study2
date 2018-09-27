#pragma once

#include "Line.h"

class DebugDrawSphere : public Line
{
public:
	DebugDrawSphere(float& radius,
		int stackCount = 20, int sliceCount = 20);
	~DebugDrawSphere();

	void Render();
	void PostRender();

	void Get(float* pRadius, int* pStackCount, int* pSliceCount) {
		pRadius = &radius;
		pStackCount = &stackCount;
		pSliceCount = &sliceCount;
	}

	void Set(float& radius);
	void Set(float& radius, D3DXCOLOR& color);

	void CreateVertex();
	void UpdateBuffer();

private:
	float radius;
	int stackCount;
	int sliceCount;

	UINT lineCount;
	D3DXVECTOR3* lines;
};