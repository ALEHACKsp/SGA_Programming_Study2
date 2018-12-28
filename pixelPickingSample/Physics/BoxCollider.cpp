#include "stdafx.h"
#include "BoxCollider.h"
#include "./Objects/GameModel.h"

BoxCollider::BoxCollider(GameModel * model)
	: Collider(model->GetTransform())
{
	shape = eBox;
	model->BoundingBox(&boundingBoxMin, &boundingBoxMax);
}

BoxCollider::BoxCollider(D3DXVECTOR3 & min, D3DXVECTOR3 & max, Transform* transform)
	: Collider(transform)
{
	boundingBoxMin = min;
	boundingBoxMax = min;
}

BoxCollider::~BoxCollider()
{
}

void BoxCollider::GetLine(D3DXMATRIX & world, vector<D3DXVECTOR3>& lines)
{
}

void BoxCollider::GetBox(D3DXVECTOR3 * min, D3DXVECTOR3 * max)
{
	D3DXVec3TransformCoord(min, &boundingBoxMin, &transform->GetWorld());
	D3DXVec3TransformCoord(max, &boundingBoxMax, &transform->GetWorld());
}

void BoxCollider::GetCorners(D3DXVECTOR3 * boxCorners)
{
	boxCorners[0] = D3DXVECTOR3(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMin.z);
	boxCorners[1] = D3DXVECTOR3(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMin.z);
	boxCorners[2] = D3DXVECTOR3(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMin.z);
	boxCorners[3] = D3DXVECTOR3(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMin.z);
	boxCorners[4] = D3DXVECTOR3(boundingBoxMin.x, boundingBoxMin.y, boundingBoxMax.z);
	boxCorners[5] = D3DXVECTOR3(boundingBoxMin.x, boundingBoxMax.y, boundingBoxMax.z);
	boxCorners[6] = D3DXVECTOR3(boundingBoxMax.x, boundingBoxMin.y, boundingBoxMax.z);
	boxCorners[7] = D3DXVECTOR3(boundingBoxMax.x, boundingBoxMax.y, boundingBoxMax.z);
}
