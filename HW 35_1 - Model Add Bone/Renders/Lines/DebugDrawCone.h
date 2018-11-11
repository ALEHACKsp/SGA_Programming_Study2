#pragma once

#include "Line.h"

class DebugDrawCone : public Line
{
public:
	DebugDrawCone(D3DXVECTOR3& center, float& height, float angle = 30.0f * D3DX_PI / 180.0f,
		int sliceCount = 36);
	~DebugDrawCone();

	void Render();
	void PostRender();

	void GetPosition(D3DXVECTOR3* pCenter, float* pRadius, float* pHeight,
		float* pAngle) {
		pCenter = &center;
		pRadius = &radius;
		pHeight = &height;
		pAngle = &angle;
	}
	void SetPosition(D3DXVECTOR3& center, float& height, float angle);

	void Set(D3DXVECTOR3& center, float& height, float& angle, D3DXCOLOR& color);

	void CreateVertex();
	void UpdateBuffer();

private:
	D3DXVECTOR3 center;
	float angle;
	float radius;
	float height;
	int sliceCount;

	UINT lineCount;
	D3DXVECTOR3* lines;
};