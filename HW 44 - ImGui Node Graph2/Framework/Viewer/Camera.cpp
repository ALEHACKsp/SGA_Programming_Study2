#include "Framework.h"
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

	View();
}

void Camera::View()
{
	// LH �޼���ǥ�� �ǹ�
	D3DXMatrixLookAtLH(&matView, &position, &(position + forward), &up);
}
