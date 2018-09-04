#include "stdafx.h"
#include "Camera.h"

Camera::Camera()
	: position(0,0,0), rotation(0,0)
	, forward(0,0,1), right(1,0,0), up(0,1,0)
{
	D3DXMatrixIdentity(&matRotation);
	D3DXMatrixIdentity(&matView);

	Move();
	Rotation();
}

Camera::~Camera()
{
}

D3DXVECTOR3 Camera::Direction(Viewport * vp, Perspective * perspective)
{
	// ȭ�� ������ �˾Ƴ����� -> viewport�� ����
	D3DXVECTOR2 screenSize;
	screenSize.x = vp->GetWidth();
	screenSize.y = vp->GetHeight();

	// 3�������� ��ȯ�Ҳ��� vector3�� �Ѱ�
	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	// ���콺�� ��ġ�� viewport�� ����ȯ�� ���
	D3DXVECTOR2 point;

	// Inv Viewport
	{
		point.x = ((2.0f * mouse.x) / screenSize.x) - 1.0f;
		point.y = (((2.0f * mouse.y) / screenSize.y) - 1.0f) * -1.0f;
	}

	// Inv Projection
	{
		D3DXMATRIX projection;
		perspective->GetMatrix(&projection);

		point.x = point.x / projection._11;
		point.y = point.y / projection._22;
	}

	D3DXVECTOR3 dir(0,0,0);
	// Inv View
	{
		D3DXMATRIX invView;
		D3DXMatrixInverse(&invView, NULL, &matView);

		//dir.x = 
		//	(point.x * invView._11) + (point.y * invView._21) + invView._31;
		//dir.x = 
		//	(point.y * invView._12) + (point.y * invView._22) + invView._32;
		//dir.x = 
		//	(point.x * invView._13) + (point.y * invView._23) + invView._33;

		// ���� ���̶� ���� z�� 1�̶�� �����ϸ�
		D3DXVec3TransformNormal
			(&dir, &D3DXVECTOR3(point.x, point.y, 1), &invView);
		D3DXVec3Normalize(&dir, &dir);
	}

	return dir;
}

D3DXVECTOR3 Camera::Direction(Viewport * vp, Perspective * perspective, D3DXVECTOR3 & mouse)
{
	// ȭ�� ������ �˾Ƴ����� -> viewport�� ����
	D3DXVECTOR2 screenSize;
	screenSize.x = vp->GetWidth();
	screenSize.y = vp->GetHeight();

	//// 3�������� ��ȯ�Ҳ��� vector3�� �Ѱ�
	//D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	// ���콺�� ��ġ�� viewport�� ����ȯ�� ���
	D3DXVECTOR2 point;

	// Inv Viewport
	{
		point.x = ((2.0f * mouse.x) / screenSize.x) - 1.0f;
		point.y = (((2.0f * mouse.y) / screenSize.y) - 1.0f) * -1.0f;
	}

	// Inv Projection
	{
		D3DXMATRIX projection;
		perspective->GetMatrix(&projection);

		point.x = point.x / projection._11;
		point.y = point.y / projection._22;
	}

	D3DXVECTOR3 dir(0, 0, 0);
	// Inv View
	{
		D3DXMATRIX invView;
		D3DXMatrixInverse(&invView, NULL, &matView);

		//dir.x = 
		//	(point.x * invView._11) + (point.y * invView._21) + invView._31;
		//dir.x = 
		//	(point.y * invView._12) + (point.y * invView._22) + invView._32;
		//dir.x = 
		//	(point.x * invView._13) + (point.y * invView._23) + invView._33;

		// ���� ���̶� ���� z�� 1�̶�� �����ϸ�
		D3DXVec3TransformNormal
		(&dir, &D3DXVECTOR3(point.x, point.y, 1), &invView);
		D3DXVec3Normalize(&dir, &dir);
	}

	return dir;
}

void Camera::Move()
{
	View();
}

void Camera::Rotation()
{
	D3DXMATRIX x, y;
	D3DXMatrixRotationX(&x, rotation.x); // radian ��
	D3DXMatrixRotationY(&y, rotation.y); // radian ��

	matRotation = x * y;

	// forward up right �����������

	D3DXVec3TransformNormal(&forward, &D3DXVECTOR3(0, 0, 1), &matRotation);
	D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1, 0, 0), &matRotation);
	D3DXVec3TransformNormal(&up, &D3DXVECTOR3(0, 1, 0), &matRotation);

}

void Camera::View()
{
	// LH �޼���ǥ�� �ǹ�
	D3DXMatrixLookAtLH(&matView, &position, &(position + forward), &up);
}
