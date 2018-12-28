#pragma once
#include "Collider.h"

class SphereCollider : public Collider
{
public:
	SphereCollider(class GameModel* model);
	SphereCollider(D3DXVECTOR3& position, float radius, Transform* transform = NULL);
	~SphereCollider();

	void GetLine(D3DXMATRIX& world, vector<D3DXVECTOR3>& lines);

	void SetPosition(D3DXVECTOR3& position);
	D3DXVECTOR3 GetPosition();

private:
	D3DXVECTOR3 position;
};