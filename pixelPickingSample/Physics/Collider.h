#pragma once
#include "./Interfaces/ILine.h"

enum Shape
{
	eRay,
	eSphere,
	eBox,
	eCapsule,
	ePlane,
	eCount
};

class Collider : public ILine
{
public:
	Collider(class Transform* transform = NULL);
	virtual ~Collider();

	virtual void GetLine(D3DXMATRIX& world, vector<D3DXVECTOR3>& lines) = 0;

	D3DXMATRIX GetWorld();
	D3DXVECTOR3 GetScale();

	Transform* GetTransform() { return transform; }
	float GetRadius() { return radius; }

	Shape GetType() { return shape; }
	void Enable(bool enable = true) { this->enable = enable; }

protected:
	Transform* transform;

	Shape shape;

	float radius;

	bool enable;
};