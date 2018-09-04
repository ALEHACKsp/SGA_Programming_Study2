#pragma once

#include "Line.h"

class DebugDrawSphere2 : public Line
{
public:
	DebugDrawSphere2(D3DXVECTOR3& center, float& radius, int sliceCount = 36);
	~DebugDrawSphere2();

	void Render();
	void PostRender();

	void GetPosition(D3DXVECTOR3* pCenter, float* prange) {
		pCenter = &center;
		prange = &range;
	}
	void SetPosition(D3DXVECTOR3& center, float& range);

	void CreateVertex();
	void UpdateBuffer();

private:
	D3DXVECTOR3 center;
	float range;
	int sliceCount;

	UINT lineCount;
	D3DXVECTOR3* lines;
};