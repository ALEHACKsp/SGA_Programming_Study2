#pragma once

#include "Collider.h"

#include "../Renders/Lines/DebugDrawSphere.h"

class SphereCollider : public Collider
{
public:
	SphereCollider(float radius = 1.0f);
	~SphereCollider();

	void Update(D3DXMATRIX& world);
	void Render();
	virtual void PostRender();	

	void Radius(float radius);
	float Radius() { return radius; }

private:
	float radius;

	DebugDrawSphere* debug;
};