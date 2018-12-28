#pragma once
#include "Collider.h"

class RayCollider : public Collider
{
public:
	RayCollider(D3DXVECTOR3& position, D3DXVECTOR3& direction, Transform* transform = NULL);
	~RayCollider();

	void GetLine(D3DXMATRIX& world, vector<D3DXVECTOR3>& lines);

	void SetDirection(D3DXVECTOR3& direction);
	D3DXVECTOR3 GetDirection();

	void SetPosition(D3DXVECTOR3& position);
	D3DXVECTOR3 GetPosition();

private:
	D3DXVECTOR3 position;
	D3DXVECTOR3 direction;
};