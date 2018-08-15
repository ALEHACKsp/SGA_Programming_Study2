#include "stdafx.h"
#include "Collision.h"

#include "RectCollider.h"

bool Collision::IsOverlap(D3DXMATRIX * matWorldA, CircleCollider * pBoundA, D3DXMATRIX * matWorldB, CircleCollider * pBoundB)
{
	D3DXVECTOR3 centerA;
	float radiusA;
	D3DXVECTOR3 centerB;
	float radiusB;

	// ���� ������ �־ ���� ó�� ���ص� ��
	pBoundA->GetWorldCenterRadius(matWorldA, &centerA, &radiusA);
	pBoundB->GetWorldCenterRadius(matWorldB, &centerB, &radiusB);

	D3DXVECTOR3 dirTo = centerB - centerA;

	float dist = D3DXVec3Length(&dirTo);

	float distPow = (dist * dist);
	float sumRadiusPow = radiusA + radiusB;
	sumRadiusPow *= sumRadiusPow;

	if (distPow > sumRadiusPow) return false;
	return true;
}

bool Collision::IsOverlap(D3DXMATRIX * matWorldA, RectCollider * pBoundA, D3DXMATRIX * matWorldB, CircleCollider * pBoundB)
{
	D3DXVECTOR3 minA, maxA;
	pBoundA->GetWorldAABBMinMax(matWorldA, &minA, &maxA);


	D3DXVECTOR3 centerB;
	float radiusB;
	pBoundB->GetWorldCenterRadius(matWorldB, &centerB, &radiusB);

	// ���� �߽����� �簢�� �𼭸��� �ƴ� �簢�� ���̿� ���� ��
	if (minA.x <= centerB.x && centerB.x <= maxA.x
		|| minA.z <= centerB.z && centerB.z <= maxA.z)
	{
		// ������ �簢������ ���� ��������ŭ Ŀ�� �簢�� ����
		D3DXVECTOR3 minT, maxT;
		minT.x = minA.x - radiusB;
		minT.y = 0;
		minT.z = minA.z - radiusB;

		maxT.x = maxA.x + radiusB;
		maxT.y = 0;
		maxT.z = maxA.z + radiusB;

		// �� �簢�� �ȿ� ���� �� ������ �浹
		if (minT.x <= centerB.x && centerB.x <= maxT.x
			&& minT.z <= centerB.z && centerB.z <= maxT.z)
			return true;
	}
	// �𼭸� �ʿ� ���� ��� �ʿ�
	else
	{
		float distance[4];
		centerB.y = 0;
		// ���� �� �𼭸�
		distance[0] = D3DXVec3Length(&(
			centerB - D3DXVECTOR3(minA.x, 0, maxA.z)));
		// ������ ��
		distance[1] = D3DXVec3Length(&(
			centerB - D3DXVECTOR3(maxA.x, 0, maxA.z)));
		// ���� �Ʒ�
		distance[2] = D3DXVec3Length(&(
			centerB - D3DXVECTOR3(minA.x, 0, minA.z)));
		// ������ �Ʒ�
		distance[3] = D3DXVec3Length(&(
			centerB - D3DXVECTOR3(maxA.x, 0, minA.z)));

		for (int i = 0; i < 4; i++)
		{
			if (distance[i] < radiusB)
				return true;
		}
	}

	return false;
}

bool Collision::IsOverlap(D3DXMATRIX * matWorldA, RectCollider * pBoundA, D3DXMATRIX * matWorldB, RectCollider * pBoundB)
{
	D3DXVECTOR3 minA, maxA;
	D3DXVECTOR3 minB, maxB;

	pBoundA->GetWorldAABBMinMax(matWorldA, &minA, &maxA);
	pBoundB->GetWorldAABBMinMax(matWorldB, &minB, &maxB);

	if (minA.x <= maxB.x && maxA.x >= minB.x && 
		minA.z <= maxB.z && maxA.z >= minB.z) {
		return true;
	}

	return false;
}
