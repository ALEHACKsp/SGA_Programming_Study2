#pragma once

#include "Line.h"

class DebugDrawSphere : public Line
{
public:
	DebugDrawSphere(D3DXVECTOR3& center, float& radius,
		int stackCount = 20, int sliceCount = 20);
	~DebugDrawSphere();

	void Render();
	void PostRender();

	void GetPosition(D3DXVECTOR3* pCenter, float* pRadius,
		int* pStackCount, int* pSliceCount) {
		pCenter = &center;
		pRadius = &radius;
		pStackCount = &stackCount;
		pSliceCount = &sliceCount;
	}
	void SetPosition(D3DXVECTOR3& center);

	void CreateVertex();
	void UpdateBuffer();

private:
	D3DXVECTOR3 center;
	float radius;
	int stackCount;
	int sliceCount;

	UINT lineCount;
	D3DXVECTOR3* lines;
};