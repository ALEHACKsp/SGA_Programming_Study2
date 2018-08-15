#include "stdafx.h"
#include "Orbit.h"

Orbit::Orbit(float moveSpeed, float rotationSpeed)
	: moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
	, targetPos(NULL)
{
}

Orbit::~Orbit() {

}

void Orbit::Update()
{
	// 회전 처리
	{
		D3DXVECTOR2 rotation;
		Rotation(&rotation);

		if (Mouse::Get()->Press(1))
		{
			// 마우스 이동값 가져오는거 얼마만큼 이동했는지의 차값
			D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();

			// 마우스 위아래로 움직이면 x축 회전이라 y인거
			rotation.x += val.y * rotationSpeed * Time::Delta();
			// 좌우회전이 y축 회전
			rotation.y += val.x * rotationSpeed * Time::Delta();

			//if (Keyboard::Get()->Press('A'))
			//	rotation.y += rotationSpeed * Time::Delta();
			//else if (Keyboard::Get()->Press('D'))
			//	rotation.y -= rotationSpeed * Time::Delta();

			//if (Keyboard::Get()->Press('W'))
			//	rotation.x += rotationSpeed * Time::Delta();
			//else if (Keyboard::Get()->Press('S'))
			//	rotation.x -= rotationSpeed * Time::Delta();

			Rotation(rotation.x, rotation.y);
		}
	}

	// 이동 처리
	{
		// 카메라 공전 => 이동 => 회전 => 공전할 목표로 이동
		D3DXMATRIX matTrans, matRot, matTargetTrans;
		D3DXVECTOR3 finalPos(0, 0, 0);
		D3DXMATRIX matFinal;

		D3DXMatrixIdentity(&matTrans);
		D3DXMatrixIdentity(&matTargetTrans);
		D3DXMatrixIdentity(&matFinal);

		if (Mouse::Get()->Press(1))
		{
			if (Keyboard::Get()->Press('Q'))
				moveValue += moveSpeed * Time::Delta();
			else if (Keyboard::Get()->Press('E'))
				moveValue -= moveSpeed * Time::Delta();
		}

		D3DXMatrixTranslation(&matTrans, 0, 0, moveValue);
		D3DXMatrixTranslation(&matTargetTrans,
			targetPos->x, targetPos->y, targetPos->z);
		MatRotation(&matRot);

		matFinal = matTrans * matRot;
		D3DXVec3TransformCoord(&finalPos, &finalPos, &matFinal);
		D3DXVec3TransformCoord(&finalPos, &finalPos, &matTargetTrans);

		Position(finalPos.x, finalPos.y, finalPos.z);
	}
}

void Orbit::SetTargetPos(D3DXVECTOR3 * targetPos)
{
	this->targetPos = targetPos;
	Position(targetPos->x, targetPos->y, targetPos->z - 50);
	moveValue = -50;
}
