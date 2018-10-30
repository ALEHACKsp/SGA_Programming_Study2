#pragma once

#include "Collider.h"
#include "../Renders/Lines/DebugDrawCapsule.h"

class CapsuleCollider : public Collider
{
public:
	enum class Axis { Axis_X, Axis_Y, Axis_Z };

	CapsuleCollider(float height = 2, float radius = 1, Axis axis = Axis::Axis_Y);
	~CapsuleCollider();

	void Update(D3DXMATRIX& world);
	void Render();
	virtual void PostRender();
		
	void Height(float height);
	float Height() { return height; }

	void Radius(float radius);
	float Radius() { return radius; }

private:
	float height;
	float radius;
	Axis axis;

	DebugDrawCapsule* debug;
};