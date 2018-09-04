#include "stdafx.h"
#include "Frustum.h"

#include "../Renders/Lines/Line.h"

Frustum::Frustum(ExecuteValues * values, float zFar, Camera * camera, Perspective * perspective)
	: values(values), zFar(zFar), camera(camera), perspective(perspective)
{
	if (camera == NULL)
		this->camera = values->MainCamera;
	
	if (perspective == NULL)
		this->perspective = values->Perspective;

	line = new Line();
	line->Init(linePos, 12);

	depthMode[0] = new DepthStencilState();
	depthMode[1] = new DepthStencilState();
	depthMode[1]->DepthEnable(false);
}

Frustum::~Frustum()
{
	SAFE_DELETE(line);

	SAFE_DELETE(depthMode[0]);
	SAFE_DELETE(depthMode[1]);
}

void Frustum::Update()
{
	// Calc Planes
	{
		D3DXMATRIX V, P;
		camera->Matrix(&V);
		perspective->GetMatrix(&P);

		float zNear = -P._43 / P._33;
		float r = zFar / (zFar - zNear);

		P._33 = r;
		P._43 = -r * zNear;


		D3DXMATRIX matrix;
		D3DXMatrixMultiply(&matrix, &V, &P);

		// Near
		// 3�� z��
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
		// ī�޶� �������� �־ +�� ����
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

		// �� ����� �� ���� ��ĵ� ����
		// V P ���ؼ� ����� ���� �� �� �������� ť�� ����� �������� �� �������� ��ȯ�ص� ��
		// �׷��� ���� ������ ���� ���� ��
		//D3DXPlaneFromPoints() // ��ġ 3�� ���� plane ���� ���� ����
		// �� ����� �ӵ��� ����
	}

	CalcCrossPoint();
}

void Frustum::Render()
{
	D3DXVECTOR3 temp[] =
	{
		// �ո�
		crossPoint[0],crossPoint[1], crossPoint[1],crossPoint[2],
		crossPoint[2],crossPoint[3], crossPoint[3],crossPoint[0],

		// �޸�
		crossPoint[4],crossPoint[5], crossPoint[5],crossPoint[6],
		crossPoint[6],crossPoint[7], crossPoint[7],crossPoint[4],

		// ���� ��
		crossPoint[0],crossPoint[4], crossPoint[1],crossPoint[5],
		// ������ ��
		crossPoint[2],crossPoint[6], crossPoint[3],crossPoint[7]
	};

	for (int i = 0; i < 24; i++)
		linePos[i] = temp[i];

	line->SetLine(linePos, 12);

	depthMode[1]->OMSetDepthStencilState();
	line->Render();
	depthMode[0]->OMSetDepthStencilState();
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

bool Frustum::ContainCube(D3DXVECTOR3 & center, float radius)
{
	D3DXVECTOR3 check; 

	for (int i = 0; i < 6; i++)
	{
		check.x = center.x - radius;
		check.y = center.y - radius;
		check.z = center.z - radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x + radius;
		check.y = center.y - radius;
		check.z = center.z - radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x - radius;
		check.y = center.y + radius;
		check.z = center.z - radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x + radius;
		check.y = center.y + radius;
		check.z = center.z - radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x - radius;
		check.y = center.y - radius;
		check.z = center.z + radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x + radius;
		check.y = center.y - radius;
		check.z = center.z + radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x - radius;
		check.y = center.y + radius;
		check.z = center.z + radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		check.x = center.x + radius;
		check.y = center.y + radius;
		check.z = center.z + radius;
		if (D3DXPlaneDotCoord(&planes[i], &check) >= 0.0f)
			continue;

		return false;
	}

	return true;
}

void Frustum::CalcCrossPoint()
{
	D3DDesc desc;
	D3D::GetDesc(&desc);

	D3DXVECTOR3 pos[4], dir[4];

	float pWidth, pHeight;
	pWidth = perspective->GetWidth();
	pHeight = perspective->GetHeight();

	D3DXVECTOR3 startPos, endPos;
	startPos = { desc.Width / 2 - pWidth / 2, desc.Height / 2 + pHeight / 2, 0 };
	endPos = { desc.Width / 2 + pWidth / 2, desc.Height / 2 - pHeight / 2, 0 };

	pos[0] = D3DXVECTOR3(startPos.x, startPos.y, 0);
	pos[1] = D3DXVECTOR3(endPos.x, startPos.y, 0);
	pos[2] = D3DXVECTOR3(endPos.x, endPos.y, 0);
	pos[3] = D3DXVECTOR3(startPos.x, endPos.y, 0);

	D3DXVECTOR3 camPos;
	camera->Position(&camPos);

	for (int i = 0; i < 4; i++) {
		dir[i] = camera->Direction(values->Viewport, values->Perspective, pos[i]);

		// ���� ���� ������ ���ϱ� ���� http://www.gisdeveloper.co.kr/?p=792
		// far plane (planes[1])�� �����̶� ������ ���غ��� -> crossPoint
		float t1, t2;
		// �� �� ����ϰ� ���� - �߸��Ἥ �����
		t1 = planes[0].a * camPos.x
			+ planes[0].b * camPos.y
			+ planes[0].c * camPos.z
			+ planes[0].d;
		t2 = planes[0].a * -dir[i].x
			+ planes[0].b * -dir[i].y
			+ planes[0].c * -dir[i].z;
		float u = t1 / t2;
		crossPoint[i] = camPos + u * dir[i];

		// near plane (planes[0])�� �����̶� ������ ���ϱ�
		t1 = planes[1].a * camPos.x
			+ planes[1].b * camPos.y
			+ planes[1].c * camPos.z
			+ planes[1].d;
		t2 = planes[1].a * -dir[i].x
			+ planes[1].b * -dir[i].y
			+ planes[1].c * -dir[i].z;
		u = t1 / t2;
		crossPoint[i + 4] = camPos + u * dir[i];
	}
}
