#include "stdafx.h"
#include "SphereCollider.h"
#include "./Objects/GameModel.h"

SphereCollider::SphereCollider(GameModel * model)
	: Collider(model->GetTransform())
{
	shape = eSphere;
}

SphereCollider::SphereCollider(D3DXVECTOR3 & position, float radius, Transform * transform)
	: Collider(transform)
	, position(position)
{
	shape = eSphere;
	this->radius = radius;
}

SphereCollider::~SphereCollider()
{
}

void SphereCollider::GetLine(D3DXMATRIX & world, vector<D3DXVECTOR3>& lines)
{
}

void SphereCollider::SetPosition(D3DXVECTOR3 & position)
{
	if (transform == NULL)
	{
		this->position = position;
	}
	else
	{
		D3DXVec3TransformCoord(&this->position, &position, &transform->GetWorldInv());
	}
}

D3DXVECTOR3 SphereCollider::GetPosition()
{
	if (transform == NULL)
	{
		return this->position;
	}
	else
	{
		D3DXVECTOR3 result;
		D3DXVec3TransformCoord(&result, &this->position, &transform->GetWorld());
		return result;
	}
}

