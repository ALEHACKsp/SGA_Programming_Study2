#include "stdafx.h"
#include "Orbit.h"

Orbit::Orbit(D3DXVECTOR3* focusPoint, float orbitRadius, float moveSpeed, float rotationSpeed)
	: focusPoint(focusPoint), moveSpeed(moveSpeed), rotationSpeed(rotationSpeed)
	, orbitRadius(orbitRadius), setFocusPoint(0, 0, 0)
{
}

Orbit::~Orbit()
{
}

void Orbit::Update()
{
	D3DXVECTOR3 position;
	Position(&position);

	//	È¸Àü
	{
		D3DXVECTOR2 rotation;
		Rotation(&rotation);

		D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();

		rotation.x += val.y * rotationSpeed * Time::Delta();

		if (rotation.x > Math::ToRadian(90))
			rotation.x = Math::ToRadian(90);
		if (rotation.x < Math::ToRadian(-90))
			rotation.x = Math::ToRadian(-90);

		rotation.y += val.x * rotationSpeed * Time::Delta();

		Rotation(rotation.x, rotation.y);
	}
	
	//ÁÜ¾Æ¿ô
	{
		if (Mouse::Get()->GetMoveValue().z > 0)
			orbitRadius -= orbitRadius * 0.1f * moveSpeed * Time::Delta() * 5;
		else if (Mouse::Get()->GetMoveValue().z < 0)
			orbitRadius += orbitRadius * 0.1f * moveSpeed * Time::Delta() * 5;

		if (orbitRadius < 0.1f)
			orbitRadius = 0.1f;
	}
	

	D3DXVECTOR3 forward;
	Forward(&forward);

	position = *focusPoint + setFocusPoint - forward * orbitRadius;
	Position(position.x, position.y, position.z);
}

void Orbit::SetFocusPoint(D3DXVECTOR3& focusPoint)
{
	setFocusPoint = focusPoint;
}
