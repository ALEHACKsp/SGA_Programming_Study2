#include "stdafx.h"
#include "Collider.h"

Collider::Collider()
	: position(0, 0, 0), scale(1, 1, 1), rotation(0, 0, 0)
	, direction(0, 0, 1), up(0, 1, 0), right(1, 0, 0)
{
	D3DXMatrixIdentity(&world);
}

void Collider::PostRender()
{
	D3DXVECTOR3 s, r, t;
	s = Scale();
	r = Rotation();
	t = Position();

	if (ImGui::DragFloat3("Position", (float*)&t, 0.05f))
		Position(t);
	if (ImGui::DragFloat3("Rotation", (float*)&r, 0.01f))
		Rotation(r);
	if (ImGui::DragFloat3("Scale", (float*)&s, 0.1f))
		Scale(s);
}

D3DXMATRIX Collider::World()
{
	return world;
}

void Collider::Position(D3DXVECTOR3 & vec)
{
	position = vec;

	UpdateWorld();
}

void Collider::Position(float x, float y, float z)
{
	Position(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 Collider::Position()
{
	return position;
}

void Collider::Scale(D3DXVECTOR3 & vec)
{
	scale = vec;

	UpdateWorld();
}

void Collider::Scale(float x, float y, float z)
{
	Scale(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 Collider::Scale()
{
	return scale;
}

void Collider::Rotation(D3DXVECTOR3 & vec)
{
	rotation = vec;

	UpdateWorld();
}

void Collider::Rotation(float x, float y, float z)
{
	Rotation(D3DXVECTOR3(x, y, z));
}

void Collider::RotationDegree(D3DXVECTOR3 & vec)
{
	D3DXVECTOR3 temp;

	temp.x = Math::ToRadian(vec.x);
	temp.y = Math::ToRadian(vec.y);
	temp.z = Math::ToRadian(vec.z);

	Rotation(temp);
}

void Collider::RotationDegree(float x, float y, float z)
{
	RotationDegree(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 Collider::Rotation()
{
	return rotation;
}

D3DXVECTOR3 Collider::RotationDegree()
{
	D3DXVECTOR3 temp;

	temp.x = Math::ToDegree(rotation.x);
	temp.y = Math::ToDegree(rotation.y);
	temp.z = Math::ToDegree(rotation.z);

	return temp;
}

D3DXVECTOR3 Collider::Direction()
{
	return direction;
}

D3DXVECTOR3 Collider::Up()
{
	return up;
}

D3DXVECTOR3 Collider::Right()
{
	return right;
}

void Collider::UpdateWorld()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(
		&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	world = S * R * T;

	direction = D3DXVECTOR3(world._31, world._32, world._33);
	up = D3DXVECTOR3(world._21, world._22, world._23);
	right = D3DXVECTOR3(world._11, world._12, world._13);
}
