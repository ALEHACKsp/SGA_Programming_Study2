#include "stdafx.h"
#include "CapsuleCollider.h"

CapsuleCollider::CapsuleCollider(float height, float radius, Axis axis)
	: height(height), radius(radius), axis(axis)
{
	type = Type::CAPSULE;

	debug = new DebugDrawCapsule(radius, height, (DebugDrawCapsule::Axis)axis);
}

CapsuleCollider::~CapsuleCollider()
{
	SAFE_DELETE(debug);
}

void CapsuleCollider::Update(D3DXMATRIX & world)
{
	D3DXMATRIX matFinal = World() * world;

	D3DXVECTOR3 S, T;
	D3DXQUATERNION R;
	D3DXMatrixDecompose(&S, &R, &T, &matFinal);
	D3DXVECTOR3 rotation;
	Math::QuatToPitchYawRoll(R, rotation.x, rotation.y, rotation.z);

	debug->Scale(S);
	debug->Rotation(rotation);
	debug->Position(T);
}

void CapsuleCollider::Render()
{
	debug->Render();
}

void CapsuleCollider::PostRender()
{
	__super::PostRender();

	if (ImGui::DragFloat("Height", &height))
		Height(height);

	if (ImGui::DragFloat("Radius", &radius))
		Radius(radius);
}

void CapsuleCollider::Height(float height)
{
	this->height = height;
	debug->Set(radius, height);
}

void CapsuleCollider::Radius(float radius)
{
	this->radius = radius;
	debug->Set(radius, height);
}
