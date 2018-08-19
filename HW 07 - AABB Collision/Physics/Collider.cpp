#include "stdafx.h"
#include "Collider.h"

#define CIRCLE_SEGMENTS 36

#include "../Renders/Line.h"

Collider::Collider(COLLIDER_TYPE type)
	: type(type)
{
	line = new Line();
}

Collider::~Collider()
{
	SAFE_DELETE(line);
}

void Collider::GetWorldBox(D3DXMATRIX * matWorld, D3DXVECTOR3 * outBoxPos)
{
	assert(type = COLLIDER_BOX);

	D3DXVECTOR3 vertices[5];

	vertices[0] = D3DXVECTOR3(localMinPos.x, 0, localMaxPos.z);
	vertices[1] = D3DXVECTOR3(localMinPos.x, 0, localMinPos.z);
	vertices[2] = D3DXVECTOR3(localMaxPos.x, 0, localMinPos.z);
	vertices[3] = D3DXVECTOR3(localMaxPos.x, 0, localMaxPos.z);
	vertices[4] = D3DXVECTOR3(localMinPos.x, 0, localMaxPos.z);

	for (int i = 0; i < 5; i++)
		D3DXVec3TransformCoord(&outBoxPos[i], &vertices[i], matWorld);

	this->CreateBoundBox();
}

void Collider::GetWorldAABBMinMax(D3DXMATRIX * matWorld, D3DXVECTOR3 * min, D3DXVECTOR3 * max)
{
	assert(type == COLLIDER_BOX);

	D3DXVECTOR3 worldPos[5];
	GetWorldBox(matWorld, worldPos);

	*min = worldPos[0];
	*max = worldPos[0];

	for (int i = 1; i < 5; i++) {
		if (min->x > worldPos[i].x)
			min->x = worldPos[i].x;
		if (min->y > worldPos[i].y)
			min->y = worldPos[i].y;
		if (min->z > worldPos[i].z)
			min->z = worldPos[i].z;

		if (max->x < worldPos[i].x)
			max->x = worldPos[i].x;
		if (max->y < worldPos[i].y)
			max->y = worldPos[i].y;
		if (max->z < worldPos[i].z)
			max->z = worldPos[i].z;
	}

	CreateAABBBox();

}

void Collider::Render(D3DXMATRIX * matWorld)
{
}

void Collider::SetBound(const D3DXVECTOR3 * pCenter, const D3DXVECTOR3 * pHalfSize)
{
	if (type == COLLIDER_BOX)
	{
		localCenter = *pCenter;
		localMinPos = localCenter - halfSize;
		localMaxPos = localCenter + halfSize;
	}
	else if (type == COLLIDER_SPHERE)
	{
		localCenter = *pCenter;
		halfSize = *pHalfSize;
		radius = D3DXVec3Length(pHalfSize);
	}
}

void Collider::GetWorldCenterRadius(D3DXMATRIX * matWorld, D3DXVECTOR3 * center, float * radius)
{
	if (type == COLLIDER_SPHERE)
	{
		D3DXVec3TransformCoord(center, &localCenter, matWorld);
		D3DXVECTOR3 scale = { matWorld->_11, matWorld->_22, matWorld->_33 };
		D3DXVECTOR3 halfSize;

		halfSize.x = this->halfSize.x * scale.x;
		halfSize.y = this->halfSize.y * scale.y;
		halfSize.z = this->halfSize.z * scale.z;

		*radius = D3DXVec3Length(&halfSize);
	}
}

void Collider::CreateBoundBox()
{
}

void Collider::CreateAABBBox()
{
}
