#include "stdafx.h"
#include "Orbit.h"

#if USINGVECTOR
Orbit::Orbit(D3DXVECTOR3 targetPos,
	float moveSpeed, float rotationSpeed)
	: moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
	, targetPos(targetPos)
{
}

#else
Orbit::Orbit(float moveSpeed, float rotationSpeed)
	: moveSpeed(moveSpeed), rotationSpeed(rotationSpeed),
	matTarget(NULL)
{
	D3DXMatrixIdentity(&mat);
}
#endif

Orbit::~Orbit()
{
}

void Orbit::Update()
{
#if USINGVECTOR // vector 계산으로 회전 및 이동 시도
	D3DXVECTOR3 forward, right, up;

	//Forward(&forward);
	//Right(&right);
	//Up(&up);

	// 이동처리 & 회전처리
	{
		D3DXVECTOR3 position;
		Position(&position);

		D3DXVECTOR3 targetDir = position - targetPos;

		D3DXVECTOR3 targetY = D3DXVECTOR3(0, 1, 0);
		D3DXVECTOR3 targetX = D3DXVECTOR3(1, 0, 0);
		D3DXVECTOR3 vec1, vec2;
		D3DXVec3Cross(&vec1, &targetDir, &targetY);
		D3DXVec3Cross(&vec2, &targetDir, &targetX);
		//D3DXVec3Normalize(&vec1, &vec1);
		//D3DXVec3Normalize(&vec2, &vec2);

		D3DXVECTOR3 forwardDir, rightDir, upDir;

		forwardDir = targetDir;
		rightDir = vec1;
		upDir = vec2;

		//Forward(&forward);
		//Right(&right);
		//Up(&up);

		D3DXVECTOR2 rotation;
		Rotation(&rotation);

		bool isLeft = false;
		bool isUp = false;
		bool isRotateY = false;
		bool isRotateX = false;

		if (Keyboard::Get()->Press('Q'))
			position += forwardDir * moveSpeed * Time::Delta();
		else if (Keyboard::Get()->Press('E'))
			position += -forwardDir * moveSpeed * Time::Delta();

		if (Keyboard::Get()->Press('A')) {
			position += rightDir * moveSpeed * Time::Delta();
			isLeft = true;
			isRotateY = true;
		}
		else if (Keyboard::Get()->Press('D')) {
			position += -rightDir * moveSpeed * Time::Delta();
			isRotateY = true;
		}

		if (Keyboard::Get()->Press('W')) {
			position += upDir * moveSpeed * Time::Delta();
			isUp = true;
			isRotateX = true;
		}
		else if (Keyboard::Get()->Press('S')) {
			position += -upDir * moveSpeed * Time::Delta();
			isRotateX = true;
		}

		D3DXVECTOR3 targetDir2 = position - targetPos;
		D3DXVec3Normalize(&targetDir, &targetDir);
		D3DXVec3Normalize(&targetDir2, &targetDir2);
		float radian = acosf(D3DXVec3Dot(&targetDir, &targetDir2));
		
		//D3DXVECTOR2 dir1 = D3DXVECTOR2(targetDir.x, targetDir.y);
		//D3DXVECTOR3 targetDir2 = position - targetPos;
		//D3DXVECTOR2 dir2 = D3DXVECTOR2(targetDir2.x, targetDir2.y);
		//float radian = asinf(D3DXVec2Dot(&dir1, &dir2) /
		//	(D3DXVec2Length(&dir1) + D3DXVec2Length(&dir2)));

		if (isRotateY) {
			if (isLeft) {
				rotation.y -= radian;
			}
			else {
				rotation.y += radian;
			}
			//rotation.y += radian;
		}
		if (isRotateX) {
			if (isUp) {
				rotation.x -= radian;
			}
			else {
				rotation.x += radian;
			}
		}

		if (rotation.y >= D3DX_PI * 2)
			rotation.y = 0;
		if (rotation.y <= -D3DX_PI * 2)
			rotation.y = 0;
		
		Position(position.x, position.y, position.z);
		Rotation(rotation.x, rotation.y);
	}

	// 회전 처리
	{
		D3DXVECTOR2 rotation;
		Rotation(&rotation);

		if (Keyboard::Get()->Down('Z')) {
			rotation.y -= 0.01f;
		}
		if (Keyboard::Get()->Down('X')) {
			rotation.y += 0.01f;
		}
		Rotation(rotation.x, rotation.y);
	}
#else // matrix로 이동 및 회전

	D3DXVECTOR2 rotation;
	Rotation(&rotation);

	if (Keyboard::Get()->Press('Q')) {
		pos.z += moveSpeed * Time::Delta();
		D3DXMatrixTranslation(&mat, pos.x, pos.y, pos.z);
	}
	else if (Keyboard::Get()->Press('E')) {
		pos.z -= moveSpeed * Time::Delta();
		D3DXMatrixTranslation(&mat, pos.x, pos.y, pos.z);
	}

	if (Keyboard::Get()->Press('A')) {
		rotation.y += rotationSpeed * Time::Delta();
	}
	else if (Keyboard::Get()->Press('D')) {
		rotation.y -= rotationSpeed * Time::Delta();
	}

	if (Keyboard::Get()->Press('W')) {
		rotation.x += rotationSpeed * Time::Delta();
	}
	else if (Keyboard::Get()->Press('S')) {
		rotation.x -= rotationSpeed * Time::Delta();
	}

	D3DXMATRIX matFinal;
	D3DXMATRIX matRotX, matRotY, matRot;
	D3DXMatrixRotationX(&matRotX, rotation.x);
	D3DXMatrixRotationY(&matRotY, rotation.y);

	matRot = matRotX * matRotY * (*matTarget);
	matFinal = mat * matRot;

	Position(matFinal._41, matFinal._42, matFinal._43);
	Rotation(rotation.x, rotation.y);
#endif
}

#if USINGVECTOR

#else
void Orbit::UpdateMatrix()
{
	//D3DXVECTOR3 position;
	//Position(&position);
	//D3DXMatrixTranslation(&mat, position.x, position.y, position.z);

	Position(&pos);
	D3DXMatrixTranslation(&mat, pos.x, pos.y, pos.z);
}

#endif
