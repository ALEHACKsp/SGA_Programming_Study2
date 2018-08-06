#include "stdafx.h"
#include "Orbit.h"

#if CASE
Orbit::Orbit(float moveSpeed, float rotationSpeed)
	:moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
	, forcusPos(0, 0, 0), orbitDist(10)
{
}
#else
Orbit::Orbit(D3DXVECTOR3 targetPos, float moveSpeed, float rotationSpeed)
	: moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
	, targetPos(targetPos)
{
}

#endif

Orbit::~Orbit() {

}

#if CASE
void Orbit::Update() {
	D3DXVECTOR3 forward, right, up;
	Forward(&forward);
	Right(&right);
	Up(&up);

	{
		D3DXVECTOR3 position;
		Position(&position);

		if (Mouse::Get()->Press(1)) {
			// 회전
			D3DXVECTOR2 rotation;
			Rotation(&rotation);

			D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();

			rotation.x += val.y * rotationSpeed * Time::Delta();
			rotation.y += val.x * rotationSpeed * Time::Delta();

			Rotation(rotation.x, rotation.y);
		}

		// 키보드 자판 위에 +,- 임
		//if (Keyboard::Get()->Press(VK_OEM_MINUS))
		if (Keyboard::Get()->Press('Q'))
			orbitDist -= moveSpeed * Time::Delta();
		//else if (Keyboard::Get()->Press(VK_OEM_PLUS))
		else if (Keyboard::Get()->Press('E'))
			orbitDist += moveSpeed * Time::Delta();

		// 거리 붙어버리지 않게 처리
		if (orbitDist < 0.1f)
			orbitDist = 0.1f;
	}

	View();
}

void Orbit::View() {
	D3DXVECTOR3 position;
	Position(&position);
	D3DXVECTOR2 rotation;
	Rotation(&rotation);
	
	D3DXMATRIX X, Y;
	D3DXMatrixRotationX(&X, rotation.x);
	D3DXMatrixRotationY(&Y, rotation.y);

	D3DXMATRIX matRot;
	MatRotation(&matRot);
	matRot = X * Y;

	D3DXVECTOR3 forward, right, up;
	Forward(&forward);
	Right(&right);
	Up(&up);

	D3DXVec3TransformNormal(&forward, &D3DXVECTOR3(0, 0, 1), &matRot);
	D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1, 0, 0), &matRot);
	D3DXVec3TransformNormal(&up, &D3DXVECTOR3(0, 1, 0), &matRot);

	position = forcusPos - forward * orbitDist;

	D3DXMATRIX matView;
	Matrix(&matView);

	D3DXMatrixLookAtLH(&matView, &position, &(position + forward), &up);

	SetMatRotation(&matRot);
	SetMatView(&matView);
}
#else
void Orbit::Update()
{
	// 회전 처리
	{
		D3DXVECTOR2 rotation;
		Rotation(&rotation);

		if (Keyboard::Get()->Press('A'))
			rotation.y += rotationSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			rotation.y -= rotationSpeed * Time::Delta();

		if (Keyboard::Get()->Press('W'))
			rotation.x += rotationSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			rotation.x -= rotationSpeed * Time::Delta();

		Rotation(rotation.x, rotation.y);
	}

	// 이동 처리
	{
		// 카메라 공전 => 이동 => 회전 => 공전할 목표로 이동
		// -> 좀더 이해 필요
		D3DXMATRIX matTrans, matRot, matTargetTrans;
		D3DXVECTOR3 finalPos(0, 0, 0);
		D3DXMATRIX matFinal;

		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matTargetTrans);
		D3DXMatrixIdentity(&matFinal);

		if (Keyboard::Get()->Press('Q'))
			moveValue += moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('E'))
			moveValue -= moveSpeed * Time::Delta();

		D3DXMatrixTranslation(&matTrans, 0, 0, moveValue);
		D3DXMatrixTranslation(&matTargetTrans,
			targetPos.x, targetPos.y, targetPos.z);
		MatRotation(&matRot);

		matFinal = matTrans * matRot;
		D3DXVec3TransformCoord(&finalPos, &finalPos, &matFinal);
		D3DXVec3TransformCoord(&finalPos, &finalPos, &matTargetTrans);

		Position(finalPos.x, finalPos.y, finalPos.z);
	}
}

#endif