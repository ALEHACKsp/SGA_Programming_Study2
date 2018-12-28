#pragma once
#include "Collider.h"

class PlaneCollider : public Collider
{
public:
	PlaneCollider(D3DXPLANE& plane, Transform* transform = NULL);
	PlaneCollider(D3DXVECTOR3& normal, float d, Transform* transform = NULL);
	PlaneCollider(D3DXVECTOR3& point, D3DXVECTOR3& normal, Transform* transform = NULL);
	PlaneCollider(D3DXVECTOR3& p1, D3DXVECTOR3& p2, D3DXVECTOR3& p3, Transform* transform = NULL);
	~PlaneCollider();

	void GetLine(D3DXMATRIX& world, vector<D3DXVECTOR3>& lines);
	
	D3DXVECTOR3 GetNormal() { return D3DXVECTOR3(plane.a, plane.b, plane.c); }
	float GetD() { return plane.d; }

private:
	D3DXPLANE plane;
};