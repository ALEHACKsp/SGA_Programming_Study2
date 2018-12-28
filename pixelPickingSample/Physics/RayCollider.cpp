#include "stdafx.h"
#include "RayCollider.h"
#include "./Objects/GameModel.h"

RayCollider::RayCollider(D3DXVECTOR3 & position, D3DXVECTOR3 & direction, Transform * transform)
	: Collider(transform)
	, position(position), direction(direction)
{
	shape = eRay;
	D3DXVec3Normalize(&this->direction, &direction);
}

RayCollider::~RayCollider()
{
}

void RayCollider::GetLine(D3DXMATRIX & world, vector<D3DXVECTOR3>& lines)
{
}

void RayCollider::SetDirection(D3DXVECTOR3 & direction)
{
	if (transform == NULL)
	{
		D3DXVec3Normalize(&this->direction, &direction);
	}
	else
	{
		D3DXVec3TransformCoord(&this->direction, &direction, &transform->GetWorldInv());
		D3DXVec3Normalize(&this->direction, &this->direction);
	}
}

D3DXVECTOR3 RayCollider::GetDirection()
{
	if (transform == NULL)
	{
		return direction;
	}
	else
	{
		D3DXVECTOR3 dir;
		D3DXVec3TransformCoord(&dir, &direction, &transform->GetWorld());
		return dir;
	}
}

void RayCollider::SetPosition(D3DXVECTOR3 & position)
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

D3DXVECTOR3 RayCollider::GetPosition()
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
