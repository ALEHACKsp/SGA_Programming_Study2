#pragma once

#include "Objects\GameRender.h"

class Gizmo : public GameRender
{
public:
	enum class Axis { X, Y, Z, XZ, XY, YZ, None };
	enum class Mode { Translate, Rotate, NonUniformScale, UniformScale };
	enum class TransformSpace { Local, World };
	enum class PivotType { ObjectCenter, SelectionCenter, WorldOrigin };

public:
	Gizmo(ExecuteValues* values);
	~Gizmo();

	void Set(class GameRender* render);
	
	bool Intersect(Ray* ray);

	void Update(Ray* ray);
	void UpdateLine();
	void Render();

	void SelectAxis(Ray* ray);

	void ResetDeltas();
	
private:
	ExecuteValues* values;
	
	D3DXCOLOR axisColor[3];
	D3DXCOLOR highlightColor;
	
	BBox* axisBox[6];
	DebugLine* axis[6];

	BSphere* axisSphere[3];

	float lineOffset;
	float halfLineOffset;
	float lineLength;
	float singleAxisThickness; 
	float multiAxisThickness;

	//class GizmoModel* model[4][3];
	class GizmoModel* translateModel[3];
	class GizmoModel* rotateModel[3];
	class GizmoModel* scaleModel[3];
	class GizmoModel* uniformScaleModel[3];

	Material* meshMaterial;
	class MeshQuad* quads[3];

	vector<class ILine*> axiss;
	DebugLine* axisLine;

	BlendState* blendMode[2];
	RasterizerState* cullMode[2];
	DepthStencilState* depthMode[2];

	class GameRender* render;

	Axis activeAxis;
	Mode activeMode;

	bool snapEnable;
	bool precisionModeEnable;
	float translationSnapValue;
	float rotationSnapValue;
	float scaleSnapValue;

	float precisionModeScale;

	D3DXVECTOR3 translationScaleSnapDelta;
	float rotationSnapDelta;

	D3DXVECTOR3 translateDelta;
	D3DXVECTOR3 scaleDelta;
	D3DXVECTOR3 rotationDelta;

	D3DXVECTOR3 lastIntersectionPosition;
	D3DXVECTOR3 intersectPosition;
	D3DXVECTOR3 delta;
	D3DXVECTOR3 tDelta;

	D3DXMATRIX rotationMatrix;

	Ray startRay;

	float radius;
	float screenScale;
	float scaleFactor;

	bool bLocal;
};