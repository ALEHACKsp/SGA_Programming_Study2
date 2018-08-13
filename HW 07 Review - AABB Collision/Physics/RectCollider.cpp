#include "stdafx.h"
#include "RectCollider.h"

#include "Line.h"

RectCollider::RectCollider()
{
	line = new Line();
}

RectCollider::~RectCollider()
{
	SAFE_DELETE(line);
}

void RectCollider::GetWorldBox(D3DXMATRIX* matWorld, D3DXVECTOR3 * outBoxPos)
{
	D3DXVECTOR3 vertices[5];

	vertices[0] = D3DXVECTOR3(localMinPos.x, 0, localMaxPos.z);
	vertices[1] = D3DXVECTOR3(localMinPos.x, 0, localMinPos.z);
	vertices[2] = D3DXVECTOR3(localMaxPos.x, 0, localMinPos.z);
	vertices[3] = D3DXVECTOR3(localMaxPos.x, 0, localMaxPos.z);
	vertices[4] = D3DXVECTOR3(localMinPos.x, 0, localMaxPos.z);

	for (int i = 0; i < 5; i++)
		D3DXVec3TransformCoord(&outBoxPos[i], &vertices[i], matWorld);
}

void RectCollider::GetWorldAABBMinMax(D3DXMATRIX * matWorld, D3DXVECTOR3 * min, D3DXVECTOR3 * max)
{
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
}

void RectCollider::Render(D3DXMATRIX * matWorld)
{
	D3DXVECTOR3 worldPos[5];
	GetWorldBox(matWorld, worldPos);

	for (int i = 0; i < 5; i++)
		worldPos[i].y = 0;

	line->Render(worldPos, 4);

	D3DXVECTOR3 minPos, maxPos;
	GetWorldAABBMinMax(matWorld, &minPos, &maxPos);

	worldPos[0] = { minPos.x, 0, minPos.z };
	worldPos[1] = { maxPos.x, 0, minPos.z };
	worldPos[2] = { maxPos.x, 0, maxPos.z }; 
	worldPos[3] = { minPos.x, 0, maxPos.z };
	worldPos[4] = { minPos.x, 0, minPos.z };

	line->Render(worldPos, 4);
}

void RectCollider::SetBound(const D3DXVECTOR3 * pCenter, const D3DXVECTOR3 * pHalfSize)
{
	CircleCollider::SetBound(pCenter, pHalfSize);
	localMinPos = localCenter - halfSize;
	localMaxPos = localCenter + halfSize;
}
