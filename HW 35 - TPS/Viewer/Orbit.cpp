#include "stdafx.h"
#include "Orbit.h"

Orbit::Orbit(float moveSpeed, float rotationSpeed)
	:moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
	, targetPos(0,0,0), orbitDist(10)
{
}

Orbit::~Orbit() {

}

void Orbit::Update() {
	// Rotation
	{
		D3DXVECTOR2 rotation;
		Rotation(&rotation);

		if (Mouse::Get()->Press(1))
		{
			D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();

			rotation.x += val.y * rotationSpeed * Time::Delta();
			rotation.y += val.x * rotationSpeed * Time::Delta();

			Rotation(rotation.x, rotation.y);
		}
	}

	// Translation
	{
		D3DXMATRIX matT, matR, matTargetT;
		D3DXVECTOR2 rotation;
		D3DXVECTOR3 finalPos = { 0, 0, 0 };
		D3DXMATRIX matFinal;

		D3DXMatrixIdentity(&matT);
		D3DXMatrixIdentity(&matTargetT);

		if (Mouse::Get()->Press(1))
		{
			if (Keyboard::Get()->Press('Q'))
				orbitDist += moveSpeed * Time::Delta();
			else if (Keyboard::Get()->Press('E'))
				orbitDist -= moveSpeed * Time::Delta();
		}

		D3DXMatrixTranslation(&matT, 0, 0, -orbitDist);
		D3DXMatrixTranslation(&matTargetT,
			targetPos.x, targetPos.y, targetPos.z);
		Rotation(&rotation);
		D3DXMatrixRotationYawPitchRoll(&matR, rotation.y, rotation.x, 0);

		matFinal = matT * matR;
		D3DXVec3TransformCoord(&finalPos, &finalPos, &matFinal);
		D3DXVec3TransformCoord(&finalPos, &finalPos, &matTargetT);

		Position(finalPos.x, finalPos.y, finalPos.z);
	}

	View();
}

void Orbit::SetTarget(D3DXVECTOR3& targetPos)
{
	this->targetPos = targetPos;
	//Position(targetPos.x, targetPos.y, targetPos.z - orbitDist);
}
