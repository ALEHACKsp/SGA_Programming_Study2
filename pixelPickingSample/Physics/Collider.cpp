#include "stdafx.h"
#include "Collider.h"
#include "./Utilities/Transform.h"
#include "./Objects/GameModel.h"

Collider::Collider(Transform * transform)
	: transform(transform)
{
}

Collider::~Collider()
{
}

D3DXMATRIX Collider::GetWorld()
{
	if (transform == NULL)
	{
		D3DXMATRIX world;
		D3DXMatrixIdentity(&world);
		return world;
	}
	else
	{
		return transform->GetWorld();
	}
}

D3DXVECTOR3 Collider::GetScale()
{
	return (transform == NULL) ? D3DXVECTOR3(1, 1, 1) : transform->GetScaleWorld();
}
