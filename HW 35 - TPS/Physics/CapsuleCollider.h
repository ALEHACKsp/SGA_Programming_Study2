#pragma once

#include "Collider.h"
#include "../Renders/Lines/DebugDrawCapsule.h"

class CapsuleCollider : public Collider
{
public:
	enum class Axis { Axis_X, Axis_Y, Axis_Z };

	CapsuleCollider(float height = 1, float radius = 1, Axis axis = Axis::Axis_Y, bool bDebug = true);
	~CapsuleCollider();

	void Update(D3DXMATRIX& world);
	void Update();
	void Render();
	virtual void PostRender();
		
	void Height(float height);
	float Height() { return height; }

	void Radius(float radius);
	float Radius() { return radius; }

	void Get(OUT D3DXVECTOR3* a, OUT D3DXVECTOR3* b, OUT float* radius);

	D3DXCOLOR Color() { return debug->GetColor(); }
	void Color(D3DXCOLOR color) { debug->SetColor(color); }

private:
	float height;
	float radius;
	Axis axis;

	bool bDebug;
	DebugDrawCapsule* debug;
	
	bool bBoneWorld;
	D3DXMATRIX boneWorld;
};