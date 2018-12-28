#pragma once
#include "Collider.h"

class CapsuleCollider : public Collider
{
public:
	CapsuleCollider(class GameModel* model);
	CapsuleCollider(float capsuleRadius, float length, Transform* transform = NULL);
	CapsuleCollider(D3DXVECTOR3 point1, D3DXVECTOR3 point2, float capsuleRadius, Transform* transform = NULL);
	~CapsuleCollider();
	
	void GetLine(D3DXMATRIX& world, vector<D3DXVECTOR3>& lines);

	void GetCapsule(D3DXVECTOR3* point1, D3DXVECTOR3* point2, float* radius);

private:
	D3DXVECTOR3 point1, point2;
	float capsuleRadius;
};