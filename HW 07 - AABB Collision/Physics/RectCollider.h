#pragma once

#include "CircleCollider.h"

class RectCollider : public CircleCollider
{
public:
	RectCollider(ExecuteValues* values);
	~RectCollider();

	void GetWorldBox(D3DXMATRIX* matWorld, D3DXVECTOR3* outBoxPos);
	void GetWorldAABBMinMax(D3DXMATRIX* matWorld,
		D3DXVECTOR3 * min, D3DXVECTOR3 * max);

	void Render(D3DXMATRIX* matWorld);

	void SetBound(const D3DXVECTOR3* pCenter,
		const D3DXVECTOR3* pHalfSize);

private:
	D3DXVECTOR3 localMinPos;
	D3DXVECTOR3 localMaxPos;
};