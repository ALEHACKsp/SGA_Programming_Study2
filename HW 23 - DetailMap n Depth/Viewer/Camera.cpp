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
	// 화면 사이즈 알아내야함 -> viewport에 있음
	D3DXVECTOR2 screenSize;
	screenSize.x = vp->GetWidth();
	screenSize.y = vp->GetHeight();

	// 3차원으로 변환할꺼라서 vector3로 한거
	D3DXVECTOR3 mouse = Mouse::Get()->GetPosition();

	// 마우스의 위치가 viewport로 역변환된 결과
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

		// 위의 식이랑 같음 z가 1이라고 생각하면
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
	D3DXMatrixRotationX(&x, rotation.x); // radian 값
	D3DXMatrixRotationY(&y, rotation.y); // radian 값

	matRotation = x * y;

	// forward up right 재계산해줘야함

	D3DXVec3TransformNormal(&forward, &D3DXVECTOR3(0, 0, 1), &matRotation);
	D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1, 0, 0), &matRotation);
	D3DXVec3TransformNormal(&up, &D3DXVECTOR3(0, 1, 0), &matRotation);

}

void Camera::View()
{
	// LH 왼손좌표계 의미
	D3DXMatrixLookAtLH(&matView, &position, &(position + forward), &up);
}
