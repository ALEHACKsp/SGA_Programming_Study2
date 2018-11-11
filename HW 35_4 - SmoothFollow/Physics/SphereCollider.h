#pragma once

#include "Collider.h"

#include "../Renders/Lines/DebugDrawSphere.h"

class SphereCollider : public Collider
{
public:
	SphereCollider(float radius = 1.0f, bool bDebug = true);
	~SphereCollider();

	void Update(D3DXMATRIX& world);
	void Update();
	void Render();
	virtual void PostRender();	

	void Radius(float radius);
	float Radius() { return radius; }

private:
	float radius;

	bool bDebug;
	DebugDrawSphere* debug;
};