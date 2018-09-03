#include "stdafx.h"
#include "PickingFrustum.h"

#include "Renders\Lines\Line.h"

PickingFrustum::PickingFrustum(ExecuteValues * values, float zFar, Camera * camera, Perspective * perspective)
	: values(values), zFar(zFar), camera(camera), perspective(perspective)
	, bUpdatePlane(true), startPos(0,0,0), endPos(0,0,0)
{
	if (camera == NULL)
		this->camera = values->MainCamera;
	
	if (perspective == NULL)
		this->perspective = values->Perspective;

	line = new Line();
	line->Init(linePos, 4, D3DXCOLOR(1, 1, 1, 1), D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	lineType = 0;
}

PickingFrustum::~PickingFrustum()
{
	SAFE_DELETE(line);
}

void PickingFrustum::Update()
{
	// select Line Type 
	if (Keyboard::Get()->Down('Z'))
	{
		lineType = (lineType + 1) % 2;
	}

	if (Mouse::Get()->Down(0))
	{
		startPos = Mouse::Get()->GetPosition();
	}
	if (Mouse::Get()->Press(0))
	{
		endPos = Mouse::Get()->GetPosition();
		bUpdatePlane = true;
	}
	if (Mouse::Get()->Up(0))
	{
		endPos = Mouse::Get()->GetPosition();
		bUpdatePlane = true;
	}

	if (bUpdatePlane == true)
	{
		D3DXMATRIX V, P;
		camera->Matrix(&V);
		perspective->GetMatrix(&P);

		// �����ִ� zNear�� ���� ������ zFar�� �̿��Ͽ� View Matrix�� zNear zFar �ٽ� ����
		float zNear = -P._43 / P._33;
		float r = zFar / (zFar - zNear);

		P._33 = r;
		P._43 = -r * zNear;

		// View Projection ��
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

		CalcCrossPoint();

		D3DXVECTOR3 camPos;
		camera->Position(&camPos);

		// Left
		D3DXPlaneFromPoints(&planes[2], &camPos, &crossPoint[0], &crossPoint[3]);
		// Right
		D3DXPlaneFromPoints(&planes[3], &camPos, &crossPoint[2], &crossPoint[1]);
		// Top
		D3DXPlaneFromPoints(&planes[4], &camPos, &crossPoint[1], &crossPoint[0]);
		// Bottom
		D3DXPlaneFromPoints(&planes[5], &camPos, &crossPoint[3], &crossPoint[2]);

		// �߿� �ð�������� ���� ���� �ȱ׷��� ������! ���� ������� �߳���
		//D3DXPlaneFromPoints(&planes[2], &camPos, &crossPoint[1], &crossPoint[0]);
		//D3DXPlaneFromPoints(&planes[3], &camPos, &crossPoint[2], &crossPoint[1]);
		//D3DXPlaneFromPoints(&planes[4], &camPos, &crossPoint[3], &crossPoint[2]);
		//D3DXPlaneFromPoints(&planes[5], &camPos, &crossPoint[0], &crossPoint[3]);

		for (int i = 0; i < 6; i++)
			D3DXPlaneNormalize(&planes[i], &planes[i]);

		bUpdatePlane = false;
	}
}

void PickingFrustum::Render()
{
	if (lineType == 0)
	{
		for (int i = 0; i < 5; i++)
			linePos[i] = i == 4 ? crossPoint[0] : crossPoint[i];
		line->SetLine(linePos, 4, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	}
	else if (lineType == 1)
	{
		D3DXVECTOR3 pos[8];
		pos[0] = crossPoint2[0];
		pos[1] = crossPoint2[1];
		pos[2] = crossPoint2[2];
		pos[3] = crossPoint2[3];

		pos[4] = crossPoint[0];
		pos[5] = crossPoint[1];
		pos[6] = crossPoint[2];
		pos[7] = crossPoint[3];

		D3DXVECTOR3 temp[] =
		{
			// �ո�
			pos[0],pos[1], pos[1],pos[2],
			pos[2],pos[3], pos[3],pos[0],

			// �޸�
			pos[4],pos[5], pos[5],pos[6],
			pos[6],pos[7], pos[7],pos[4],

			// ���� ��
			pos[0],pos[4], pos[1],pos[5],
			// ������ ��
			pos[2],pos[6], pos[3],pos[7]
		};

		for (int i = 0; i < 24; i++)
			linePos2[i] = temp[i];

		line->SetLine(linePos2, 12, D3D11_PRIMITIVE_TOPOLOGY_LINELIST, true);
	}
	line->Render();

	// Test
	//for (int i = 0; i < 4; i++)
	//	ImGui::Text("%d's %f %f %f", i, crossPoint[i].x, crossPoint[i].y, crossPoint[i].z);
}

bool PickingFrustum::ContainPoint(D3DXVECTOR3 & position)
{
	if (startPos == endPos) return false;

	for (int i = 0; i < 6; i++)
	{
		if (D3DXPlaneDotCoord(&planes[i], &position) < 0.0f)
			return false;
	}

	return true;
}

void PickingFrustum::CalcCrossPoint()
{
	D3DXVECTOR3 mousePos[4], mouseDir[4];
	
	//if (startPos.x > endPos.x || startPos.y < endPos.y) {
	//	D3DXVECTOR3 temp = startPos;
	//	startPos = endPos;
	//	endPos = temp;
	//}

	mousePos[0] = D3DXVECTOR3(startPos.x, startPos.y, 0);
	mousePos[1] = D3DXVECTOR3(endPos.x, startPos.y, 0);
	mousePos[2] = D3DXVECTOR3(endPos.x, endPos.y, 0);
	mousePos[3] = D3DXVECTOR3(startPos.x, endPos.y, 0);

	D3DXVECTOR3 camPos; 
	camera->Position(&camPos);

	for (int i = 0; i < 4; i++) {
		mouseDir[i] = camera->Direction(values->Viewport, values->Perspective, mousePos[i]);

		// ���� ���� ������ ���ϱ� ���� http://www.gisdeveloper.co.kr/?p=792
		// far plane (planes[1])�� ���콺 �����̶� ������ ���غ��� -> crossPoint
		float t1, t2;
		// �� �� ����ϰ� ���� - �߸��Ἥ �����
		t1 = planes[1].a * camPos.x
			+ planes[1].b * camPos.y
			+ planes[1].c * camPos.z
			+ planes[1].d;
		t2 = planes[1].a * -mouseDir[i].x
			+ planes[1].b * -mouseDir[i].y
			+ planes[1].c * -mouseDir[i].z;
		float u = t1 / t2;
		crossPoint[i] = camPos + u * mouseDir[i];

		///////////////////////////////////////////////////////////////////////
		// linetype 1�� ��� near�� crossPoint ��� -> �׳� ���� �������
		if (true)//lineType == 1)
		{
			// ���� ���� ������ ���ϱ� ���� http://www.gisdeveloper.co.kr/?p=792
			// far plane (planes[1])�� ���콺 �����̶� ������ ���غ��� -> crossPoint
			float t1, t2;
			// �� �� ����ϰ� ���� - �߸��Ἥ �����
			t1 = planes[0].a * camPos.x
				+ planes[0].b * camPos.y
				+ planes[0].c * camPos.z
				+ planes[0].d;
			t2 = planes[0].a * -mouseDir[i].x
				+ planes[0].b * -mouseDir[i].y
				+ planes[0].c * -mouseDir[i].z;
			float u = t1 / t2;
			crossPoint2[i] = camPos + u * mouseDir[i];
		}
	}
}
