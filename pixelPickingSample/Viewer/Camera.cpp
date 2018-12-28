#include "stdafx.h"
#include "Camera.h"
#include "./Physics/RayCollider.h"

Camera::Camera()
	: position(0, 0, 0), rotation(0, 0)
	, forward(0, 0, 1), right(1, 0, 0), up(0, 1, 0)
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
	return Direction(vp, perspective, Mouse::Get()->GetPosition());
}

D3DXVECTOR3 Camera::Direction(Viewport * vp, Perspective * perspective, D3DXVECTOR3& screenPoint)
{
	D3DXVECTOR3 screenSize;
	screenSize.x = vp->GetWidth();
	screenSize.y = vp->GetHeight();

	D3DXVECTOR2 point; // 뷰포트 역변환 된 결과

					   //Inv Viewport
	{
		point.x = (2.0f * screenPoint.x) / screenSize.x - 1.0f;
		point.y = ((2.0f * screenPoint.y) / screenSize.y - 1.0f) * -1.0f;
	}

	//Inv Projection
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

		//dir.x = (point.x * invView._11) + (point.y * invView._21) + invView._31;
		//dir.y = (point.x * invView._12) + (point.y * invView._22) + invView._32;
		//dir.z = (point.x * invView._13) + (point.y * invView._23) + invView._33;
		// 위 식이 아래식으로 됨. 왜냐하면 트랜스폼노멀에서 z를 1로 줬을 때와 같기 때문.
		D3DXVec3TransformNormal(&dir, &D3DXVECTOR3(point.x, point.y, 1), &invView);
		D3DXVec3Normalize(&dir, &dir);
	}

	return dir;
}

RayCollider * Camera::ConvertMouseToRay(Viewport * vp, Perspective * perspective, D3DXVECTOR3 & screenPoint)
{
	D3DXVECTOR3 screenSize;
	screenSize.x = vp->GetWidth();
	screenSize.y = vp->GetHeight();

	D3DXVECTOR2 point; // 뷰포트 역변환 된 결과

					   //Inv Viewport
	{
		point.x = (2.0f * screenPoint.x) / screenSize.x - 1.0f;
		point.y = ((2.0f * screenPoint.y) / screenSize.y - 1.0f) * -1.0f;
	}

	//Inv Projection
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

		//dir.x = (point.x * invView._11) + (point.y * invView._21) + invView._31;
		//dir.y = (point.x * invView._12) + (point.y * invView._22) + invView._32;
		//dir.z = (point.x * invView._13) + (point.y * invView._23) + invView._33;
		// 위 식이 아래식으로 됨. 왜냐하면 트랜스폼노멀에서 z를 1로 줬을 때와 같기 때문.
		D3DXVec3TransformNormal(&dir, &D3DXVECTOR3(point.x, point.y, 1), &invView);
		D3DXVec3Normalize(&dir, &dir);
	}

	RayCollider* ray = new RayCollider(position, dir);
	return ray;
}

RECT Camera::ScreenPosition(Viewport * vp, Perspective * perspective, D3DXMATRIX & transform, UINT width, UINT height)
{
	D3DXMATRIX projection;
	perspective->GetMatrix(&projection);
	D3DXMATRIX matrix = transform * matView * projection;
	D3DXVECTOR2 point((matrix._41 / matrix._43 + 1) * vp->GetWidth() / 2, (-matrix._42 / matrix._43 + 1) * vp->GetHeight() / 2);

	RECT rect;
	rect.left = (long)point.x;
	rect.right = (long)(point.x + width);
	rect.top = (long)point.y;
	rect.bottom = (long)(point.y + height);

	return rect;
}

void Camera::Move()
{
	View();
}

void Camera::Rotation()
{
	D3DXMATRIX x, y;
	D3DXMatrixRotationX(&x, rotation.x);
	D3DXMatrixRotationY(&y, rotation.y);

	matRotation = x * y;

	D3DXVec3TransformNormal(&forward, &D3DXVECTOR3(0, 0, 1), &matRotation);
	D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1, 0, 0), &matRotation);
	D3DXVec3TransformNormal(&up, &D3DXVECTOR3(0, 1, 0), &matRotation);
}

void Camera::View()
{
	D3DXMatrixLookAtLH(&matView, &position, &(position + forward), &up);
}
