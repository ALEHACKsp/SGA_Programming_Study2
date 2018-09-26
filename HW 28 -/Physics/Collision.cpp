#include "stdafx.h"
#include "Collision.h"	

bool Collision::IsOverlapCapsuleCapsule(D3DXMATRIX * world1, CapsuleCollider * collider1, D3DXMATRIX * world2, CapsuleCollider * collider2)
{
	float s, t;
	D3DXVECTOR3 c1, c2;

	D3DXVECTOR3 a1, b1, a2, b2;
	a1 = D3DXVECTOR3(0,  collider1->Height() / 2.0f, 0);
	b1 = D3DXVECTOR3(0, -collider1->Height() / 2.0f, 0);
	a2 = D3DXVECTOR3(0,  collider2->Height() / 2.0f, 0);
	b2 = D3DXVECTOR3(0, -collider2->Height() / 2.0f, 0);

	D3DXMATRIX mat1, mat2;
	mat1 = collider1->World() * (*world1);
	mat2 = collider2->World() * (*world2);

	D3DXVECTOR3 S1, T1;
	D3DXVECTOR3 S2, T2;
	D3DXQUATERNION R1, R2;
	D3DXMatrixDecompose(&S1, &R1, &T1, &mat1);
	D3DXMatrixDecompose(&S2, &R2, &T2, &mat2);

	D3DXVec3TransformCoord(&a1, &a1, &mat1);
	D3DXVec3TransformCoord(&b1, &b1, &mat1);
	D3DXVec3TransformCoord(&a2, &a2, &mat2);
	D3DXVec3TransformCoord(&b2, &b2, &mat2);

	float dist2 = Math::ClosestPtSegmentSegment(a1, b1, a2, b2, s, t, c1, c2);
	float radius = collider1->Radius() * S1.x + collider2->Radius() * S2.x;

	bool result = dist2 <= (radius * radius);

	if (result == true)
		int a = 10;

	return result;
}
