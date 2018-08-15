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
	// ȸ�� ó��
	{
		D3DXVECTOR2 rotation;
		Rotation(&rotation);

		if (Mouse::Get()->Press(1))
		{
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
	}

	// �̵� ó��
	{
		// ī�޶� ���� => �̵� => ȸ�� => ������ ��ǥ�� �̵�
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