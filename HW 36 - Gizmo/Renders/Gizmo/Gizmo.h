#pragma once

#include "Objects\GameRender.h"

class Gizmo : public GameRender
{
public:
	enum class Axis { X, Y, Z, XY, ZX, YZ, None };
	enum class Mode { Translate, Rotate, NonUniformScale, UniformScale };
	enum class TransformSpace { Local, World };
	enum class PivotType { ObjectCenter, SelectionCenter, WorldOrigin };

public:
	Gizmo(ExecuteValues* values);
	~Gizmo();

	void Set(class GameRender* render);
	
	void Update(Ray* ray);
	void UpdateLine();
	void Render();

private:
	ExecuteValues* values;
	
	D3DXCOLOR axisColor[3];
	D3DXCOLOR highlightColor;
	
	BBox* xAxisBox;
	BBox* yAxisBox;
	BBox* zAxisBox;

	float lineOffset;
	float halfLineOffset;
	float lineLength;
	float singleAxisThickness; 

	vector<class ILine*> axiss;
	DebugLine* axisLine;

	DepthStencilState* depthMode[2];

	class GameRender* render;

	DebugLine* axis[3];
};