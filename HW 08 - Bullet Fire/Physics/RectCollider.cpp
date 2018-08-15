#include "stdafx.h"
#include "RectCollider.h"

#include "../Renders/Line.h"

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
	D3DXVECTOR3 vertices[8];

	// ¾Õ - ÁÂÇÏ ÁÂ»ó ¿ìÇÏ ¿ì»ó
	vertices[0] = D3DXVECTOR3(localMinPos.x, localMinPos.y, localMinPos.z);
	vertices[1] = D3DXVECTOR3(localMinPos.x, localMaxPos.y, localMinPos.z);
	vertices[2] = D3DXVECTOR3(localMaxPos.x, localMinPos.y, localMinPos.z);
	vertices[3] = D3DXVECTOR3(localMaxPos.x, localMaxPos.y, localMinPos.z);
	
	// µÚ - ÁÂÇÏ ÁÂ»ó ¿ìÇÏ ¿ì»ó
	vertices[4] = D3DXVECTOR3(localMinPos.x, localMinPos.y, localMaxPos.z);
	vertices[5] = D3DXVECTOR3(localMinPos.x, localMaxPos.y, localMaxPos.z);
	vertices[6] = D3DXVECTOR3(localMaxPos.x, localMinPos.y, localMaxPos.z);
	vertices[7] = D3DXVECTOR3(localMaxPos.x, localMaxPos.y, localMaxPos.z);

	for (int i = 0; i < 8; i++)
		D3DXVec3TransformCoord(&outBoxPos[i], &vertices[i], matWorld);
}

void RectCollider::GetWorldAABBMinMax(D3DXMATRIX * matWorld, D3DXVECTOR3 * min, D3DXVECTOR3 * max)
{
	D3DXVECTOR3 worldPos[8];
	GetWorldBox(matWorld, worldPos);

	*min = worldPos[0];
	*max = worldPos[0];

	for (int i = 1; i < 8; i++) {
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
	D3DXVECTOR3 worldPos[8];
	GetWorldBox(matWorld, worldPos);

	D3DXVECTOR3 lines[] =
	{
		// ¾Õ¸é
		worldPos[0],worldPos[1], worldPos[1],worldPos[3],
		worldPos[3],worldPos[2], worldPos[2],worldPos[0],

		// µÞ¸é
		worldPos[4],worldPos[5], worldPos[5],worldPos[7],
		worldPos[7],worldPos[6], worldPos[6],worldPos[4],

		// ¿ÞÂÊ ¿·
		worldPos[0],worldPos[4], worldPos[1],worldPos[5],
		// ¿À¸¥ÂÊ ¿·
		worldPos[2],worldPos[6], worldPos[3],worldPos[7]
	};

	line->Render(lines, 12, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	D3DXVECTOR3 minPos, maxPos;
	GetWorldAABBMinMax(matWorld, &minPos, &maxPos);

	worldPos[0] = { minPos.x, minPos.y, minPos.z };
	worldPos[1] = { minPos.x, maxPos.y, minPos.z };
	worldPos[2] = { maxPos.x, minPos.y, minPos.z };
	worldPos[3] = { maxPos.x, maxPos.y, minPos.z };
	worldPos[4] = { minPos.x, minPos.y, maxPos.z };
	worldPos[5] = { minPos.x, maxPos.y, maxPos.z };
	worldPos[6] = { maxPos.x, minPos.y, maxPos.z };
	worldPos[7] = { maxPos.x, maxPos.y, maxPos.z };

	D3DXVECTOR3 linesAABB[] =
	{
		// ¾Õ¸é
		worldPos[0],worldPos[1], worldPos[1],worldPos[3],
		worldPos[3],worldPos[2], worldPos[2],worldPos[0],

		// µÞ¸é
		worldPos[4],worldPos[5], worldPos[5],worldPos[7],
		worldPos[7],worldPos[6], worldPos[6],worldPos[4],

		// ¿ÞÂÊ ¿·
		worldPos[0],worldPos[4], worldPos[1],worldPos[5],
		// ¿À¸¥ÂÊ ¿·
		worldPos[2],worldPos[6], worldPos[3],worldPos[7]
	};

	line->Render(linesAABB, 12, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void RectCollider::SetBound(const D3DXVECTOR3 * pCenter, const D3DXVECTOR3 * pHalfSize)
{
	CircleCollider::SetBound(pCenter, pHalfSize);
	localMinPos = localCenter - halfSize;
	localMaxPos = localCenter + halfSize;
}
