#pragma once
#include "Execute.h"
#include "../Fbx/Exporter.h"

class TestCollision : public Execute
{
public:
	TestCollision(ExecuteValues* values);
	~TestCollision();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	class CapsuleCollider* capsule;
	class SphereCollider* sphere;

	D3DXVECTOR3 origin, direction;
	class Ray* ray;

	bool bSphereCollision;
	bool bCapsuleCollision;
};