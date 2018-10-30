#pragma once

#include "Line.h"

class DebugDrawCone2 : public Line
{
public:
	DebugDrawCone2(D3DXVECTOR3& center, float& radius, float& height,
		int sliceCount = 36);
	~DebugDrawCone2();

	void Render();
	void PostRender();

	void GetPosition(D3DXVECTOR3* pCenter, float* pRadius, float* pHeight,
		D3DXVECTOR3* pDir) {
		pCenter = &center;
		pRadius = &radius;
		pHeight = &height;
		pDir = &dir;
	}
	void SetPosition(D3DXVECTOR3& center, float& radius, float& height);

	void Set(D3DXVECTOR3& center, float& radius, float& height, D3DXCOLOR& color);

	void CreateVertex();
	void UpdateBuffer();

private:
	D3DXVECTOR3 center;
	float radius;
	float height;
	int sliceCount;

	D3DXVECTOR3 dir;

	UINT lineCount;
	D3DXVECTOR3* lines;
};