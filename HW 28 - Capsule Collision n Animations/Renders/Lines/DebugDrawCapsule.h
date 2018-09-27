#pragma once

#include "Line.h"

class DebugDrawCapsule : public Line
{
public:
	enum class Axis { Axis_X, Axis_Y, Axis_Z };

	DebugDrawCapsule(float& radius, float height, Axis axis = Axis::Axis_Y, int stackCount = 20, int sliceCount = 20);
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

	Axis axis;

	UINT lineCount;
	D3DXVECTOR3* lines;
};