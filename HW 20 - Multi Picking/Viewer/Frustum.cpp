#include "stdafx.h"
#include "Frustum.h"

Frustum::Frustum(ExecuteValues * values, float zFar, Camera * camera, Perspective * perspective)
	: values(values), zFar(zFar), camera(camera), perspective(perspective)
{
	if (camera == NULL)
		this->camera = values->MainCamera;
	
	if (perspective == NULL)
		this->perspective = values->Perspective;
}

Frustum::~Frustum()
{
}

void Frustum::Update()
{
	D3DXMATRIX V, P;
	camera->Matrix(&V);
	perspective->GetMatrix(&P);
	
	// 원래있던 zNear와 내가 설정한 zFar를 이용하여 View Matrix의 zNear zFar 다시 설정
	float zNear = -P._43 / P._33;
	float r = zFar / (zFar - zNear);

	P._33 = r;
	P._43 = -r * zNear;
	
	// View Projection 곱
	D3DXMATRIX matrix;
	D3DXMatrixMultiply(&matrix, &V, &P);

	// Near
	// 3열 z값
	planes[0].a = matrix._14 + matrix._13;
	planes[0].b = matrix._24 + matrix._23;
	planes[0].c = matrix._34 + matrix._33;
	planes[0].d = matrix._44 + matrix._43;

	// Far
	planes[1].a = matrix._14 - matrix._13;
	planes[1].b = matrix._24 - matrix._23;
	planes[1].c = matrix._34 - matrix._33;
	planes[1].d = matrix._44 - matrix._43;

	// Left
	// 카메라 뒤집어져 있어서 +가 왼쪽
	planes[2].a = matrix._14 + matrix._11;
	planes[2].b = matrix._24 + matrix._21;
	planes[2].c = matrix._34 + matrix._31;
	planes[2].d = matrix._44 + matrix._41;

	// Right
	planes[3].a = matrix._14 - matrix._11;
	planes[3].b = matrix._24 - matrix._21;
	planes[3].c = matrix._34 - matrix._31;
	planes[3].d = matrix._44 - matrix._41;

	// Top
	planes[4].a = matrix._14 - matrix._12;
	planes[4].b = matrix._24 - matrix._22;
	planes[4].c = matrix._34 - matrix._32;
	planes[4].d = matrix._44 - matrix._42;

	// Bottom
	planes[5].a = matrix._14 + matrix._12;
	planes[5].b = matrix._24 + matrix._22;
	planes[5].c = matrix._34 + matrix._32;
	planes[5].d = matrix._44 + matrix._42;

	for (int i = 0; i < 6; i++)
		D3DXPlaneNormalize(&planes[i], &planes[i]);

	// 면 만드는 더 쉬운 방식도 있음
	// V P 곱해서 역행렬 취한 후 그 공간으로 큐브 모양을 만들어놓고 그 공간으로 변환해도 됨
	// 그렇게 나온 점으로 면을 만들어도 됨
	//D3DXPlaneFromPoints() // 위치 3개 만들어서 plane 만들 수도 있음
	// 이 방법은 속도가 느림
}

bool Frustum::ContainPoint(D3DXVECTOR3 & position)
{
	for (int i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&planes[i], &position) < 0.0f)
			return false;
	}

	return true;
}
