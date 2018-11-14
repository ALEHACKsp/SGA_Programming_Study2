#pragma once

#include "Line.h"

class DebugDrawSphere2 : public Line
{
public:
	DebugDrawSphere2(float& radius, int sliceCount = 36);
	~DebugDrawSphere2();

	void Render();
	void PostRender();

	void Get(float* pRange) {
		pRange = &range;
	}
	void Set(float& range);

	void CreateVertex();
	void UpdateBuffer();

private:
	float range;
	int sliceCount;

	UINT lineCount;
	D3DXVECTOR3* lines;
};