#include "stdafx.h"
#include "CircleCollider.h"

CircleCollider::CircleCollider(ExecuteValues * values)
{
	line = new ExeLine(values);
}

CircleCollider::~CircleCollider()
{
	SAFE_DELETE(line);
}

void CircleCollider::GetWorldCenterRadius(D3DXMATRIX * matWorld,
	D3DXVECTOR3 * center, float * radius)
{
	D3DXVec3TransformCoord(center, &localCenter, matWorld);
	D3DXVECTOR3 scale = { matWorld->_11, matWorld->_22, matWorld->_33 };
	D3DXVECTOR3 halfSize;
	
	halfSize.x = this->halfSize.x * scale.x;
	halfSize.y = this->halfSize.y * scale.y;
	halfSize.z = this->halfSize.z * scale.z;

	*radius = D3DXVec3Length(&halfSize);
}

void CircleCollider::Render(D3DXMATRIX * matWorld)
{
	D3DXVECTOR3 center;
	float radius;

	this->GetWorldCenterRadius(matWorld, &center, &radius);

	float intervalAngle = (D3DX_PI * 2.0f) / CIRCLE_SEGMENTS;
	D3DXVECTOR3 circlePos[CIRCLE_SEGMENTS + 1];

	for (UINT i = 0; i <= CIRCLE_SEGMENTS; i++) {
		float angle = i * intervalAngle;
		circlePos[i] = D3DXVECTOR3(
			center.x + cosf(angle) * radius, 
			0, center.z + sinf(angle) * radius);
	}

	line->Render(circlePos, CIRCLE_SEGMENTS, 
		D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

void CircleCollider::SetBound(const D3DXVECTOR3 * pCenter, const D3DXVECTOR3 * pHalfSize)
{
	localCenter = *pCenter;
	halfSize = *pHalfSize;
	radius = D3DXVec3Length(pHalfSize);
}
