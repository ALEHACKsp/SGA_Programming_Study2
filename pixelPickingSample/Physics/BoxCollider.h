#pragma once
#include "Collider.h"

class BoxCollider : public Collider
{
public:
	BoxCollider(class GameModel* model);
	BoxCollider(D3DXVECTOR3& min, D3DXVECTOR3& max, Transform* transform = NULL);
	~BoxCollider();

	void GetLine(D3DXMATRIX& world, vector<D3DXVECTOR3>& lines);

	void GetBox(D3DXVECTOR3 * min, D3DXVECTOR3 * max);
	void GetCorners(D3DXVECTOR3* boxCorners);

private:
	D3DXVECTOR3 boundingBoxMin;
	D3DXVECTOR3 boundingBoxMax;
};