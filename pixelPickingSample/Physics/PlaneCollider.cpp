#include "stdafx.h"
#include "PlaneCollider.h"
#include "./Objects/GameModel.h"

PlaneCollider::PlaneCollider(D3DXPLANE & plane, Transform* transform)
	: Collider(transform)
	, plane(plane)
{
	shape = ePlane;
}

PlaneCollider::PlaneCollider(D3DXVECTOR3 & normal, float d, Transform* transform)
	: Collider(transform)
{
	shape = ePlane;
	plane.a = normal.x;
	plane.b = normal.y;
	plane.c = normal.z;
	plane.d = d;
}

PlaneCollider::PlaneCollider(D3DXVECTOR3 & point, D3DXVECTOR3 & normal, Transform* transform)
	: Collider(transform)
{
	shape = ePlane;
	D3DXPlaneFromPointNormal(&plane, &point, &normal);
}

PlaneCollider::PlaneCollider(D3DXVECTOR3 & p1, D3DXVECTOR3 & p2, D3DXVECTOR3 & p3, Transform* transform)
	: Collider(transform)
{
	shape = ePlane;
	D3DXPlaneFromPoints(&plane, &p1, &p2, &p3);
}

PlaneCollider::~PlaneCollider()
{
}

void PlaneCollider::GetLine(D3DXMATRIX & world, vector<D3DXVECTOR3>& lines)
{
}
