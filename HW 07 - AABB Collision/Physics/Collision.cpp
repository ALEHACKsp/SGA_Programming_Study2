#include "stdafx.h"
#include "Collision.h"

#include "RectCollider.h"

bool Collision::IsOverlapCircleCircle(D3DXMATRIX * matWorldA, CircleCollider * pBoundA, D3DXMATRIX * matWorldB, CircleCollider * pBoundB)
{
	D3DXVECTOR3 centerA;
	float radiusA;
	D3DXVECTOR3 centerB;
	float radiusB;

	// 같은 공간에 있어서 따로 처리 안해도 됨
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

bool Collision::IsOverlapRectCircle(D3DXMATRIX * matWorldA, RectCollider * pBoundA, D3DXMATRIX * matWorldB, CircleCollider * pBoundB)
{
	D3DXVECTOR3 minA, maxA;
	pBoundA->GetWorldAABBMinMax(matWorldA, &minA, &maxA);


	D3DXVECTOR3 centerB;
	float radiusB;
	pBoundB->GetWorldCenterRadius(matWorldB, &centerB, &radiusB);

	// 원의 중심점이 사각형 모서리가 아닌 사각형 사이에 있을 때
	if (minA.x <= centerB.x && centerB.x <= maxA.x
		|| minA.z <= centerB.z && centerB.z <= maxA.z)
	{
		// 기존의 사각형에서 원의 반지름만큼 커진 사각형 만듬
		D3DXVECTOR3 minT, maxT;
		minT.x = minA.x - radiusB;
		minT.y = 0;
		minT.z = minA.z - radiusB;

		maxT.x = maxA.x + radiusB;
		maxT.y = 0;
		maxT.z = maxA.z + radiusB;

		// 그 사각형 안에 원이 들어가 있으면 충돌
		if (minT.x <= centerB.x && centerB.x <= maxT.x
			&& minT.z <= centerB.z && centerB.z <= maxT.z)
			return true;
	}
	// 모서리 쪽에 대한 계산 필요
	else
	{
		float distance[4];
		centerB.y = 0;
		// 왼쪽 위 모서리
		distance[0] = D3DXVec3Length(&(
			centerB - D3DXVECTOR3(minA.x, 0, maxA.z)));
		// 오른쪽 위
		distance[1] = D3DXVec3Length(&(
			centerB - D3DXVECTOR3(maxA.x, 0, maxA.z)));
		// 왼쪽 아래
		distance[2] = D3DXVec3Length(&(
			centerB - D3DXVECTOR3(minA.x, 0, minA.z)));
		// 오른쪽 아래
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

bool Collision::IsOverlapRectRect(D3DXMATRIX * matWorldA, RectCollider * pBoundA, D3DXMATRIX * matWorldB, RectCollider * pBoundB)
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
