#include "stdafx.h"
#include "CapsuleCollider.h"
#include "./Objects/GameModel.h"

CapsuleCollider::CapsuleCollider(GameModel * model)
	: Collider(model->GetTransform())
{
	shape = eCapsule;

	//반지름 설정
	D3DXVECTOR3 minBox, maxBox;
	model->BoundingBox(&minBox, &maxBox);
	D3DXVECTOR3 center((maxBox.x + minBox.x) / 2.0f, (maxBox.y + minBox.y) / 2.0f, (maxBox.z + minBox.z) / 2.0f);
	
	capsuleRadius = max((maxBox.x - minBox.x) / 2.0f, (maxBox.z - minBox.z) / 2.0f);
	radius = max((maxBox.y - minBox.y) / 2.0f, capsuleRadius);

	//길이 설정
	float point1Y = (maxBox.y - capsuleRadius) > center.y ? maxBox.y - capsuleRadius : center.y;
	float point2Y = (minBox.y + capsuleRadius) < center.y ? minBox.y + capsuleRadius : center.y;

	point1 = D3DXVECTOR3(center.x, point1Y, center.z);
	point2 = D3DXVECTOR3(center.x, point2Y, center.z);
}

CapsuleCollider::CapsuleCollider(float capsuleRadius, float length, Transform * transform)
	: Collider(transform), capsuleRadius(capsuleRadius)
{
	shape = eCapsule;

	if (length < 0) length = 0;

	float halfLength = length / 2.0f;

	point1 = D3DXVECTOR3(0, halfLength, 0);
	point2 = D3DXVECTOR3(0, -halfLength, 0);

	this->radius = halfLength + capsuleRadius;
}

CapsuleCollider::CapsuleCollider(D3DXVECTOR3 point1, D3DXVECTOR3 point2, float capsuleRadius, Transform * transform)
	: Collider(transform)
	, point1(point1), point2(point2), capsuleRadius(capsuleRadius)
{
}

CapsuleCollider::~CapsuleCollider()
{
}

void CapsuleCollider::GetLine(D3DXMATRIX & world, vector<D3DXVECTOR3>& lines)
{
}

void CapsuleCollider::GetCapsule(D3DXVECTOR3 * point1, D3DXVECTOR3 * point2, float * radius)
{
	if (transform == NULL)
	{
		*point1 = this->point1;
		*point2 = this->point2;
		*radius = this->capsuleRadius;
	}
	else
	{
		D3DXMATRIX world = transform->GetWorld();
		{
			D3DXVECTOR3 scale = transform->GetScaleWorld();
			float scaleAvg = (scale.x + scale.y + scale.z) / 3.0f;
			*radius = this->capsuleRadius * scaleAvg;
		}
		D3DXVec3TransformCoord(point1, &this->point1, &world);
		D3DXVec3TransformCoord(point2, &this->point2, &world);
	}
}