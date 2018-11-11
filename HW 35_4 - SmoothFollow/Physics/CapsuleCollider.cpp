#include "stdafx.h"
#include "CapsuleCollider.h"

CapsuleCollider::CapsuleCollider(float height, float radius, Axis axis, bool bDebug)
	: height(height), radius(radius), axis(axis), bDebug(bDebug)
{
	type = Type::CAPSULE;

	if (bDebug)
		debug = new DebugDrawCapsule(radius, height, (DebugDrawCapsule::Axis)axis);
	else
		debug = NULL;
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

	if (bDebug) {
		debug->Scale(S);
		debug->Rotation(rotation);
		debug->Position(T);
	}
}

void CapsuleCollider::Update()
{
	if (bDebug) {
		debug->Scale(Scale());
		debug->Rotation(Rotation());
		debug->Position(Position());
	}
}

void CapsuleCollider::Render()
{
	if(bDebug)
		debug->Render();
}

void CapsuleCollider::PostRender()
{
	__super::PostRender();

	D3DXVECTOR3 s, r, t;
	s = Scale();
	r = Rotation();
	t = Position();

	if (ImGui::DragFloat3("Position##Capsule", (float*)&t, 0.05f))
		Position(t);
	if (ImGui::DragFloat3("Rotation##Capsule", (float*)&r, 0.01f))
		Rotation(r);
	if (ImGui::DragFloat3("Scale##Capsule", (float*)&s, 0.1f))
		Scale(s);

	if (ImGui::DragFloat("Height##Capsule", &height))
		Height(height);

	if (ImGui::DragFloat("Radius##Capsule", &radius))
		Radius(radius);

	if (bDebug) {
		D3DXCOLOR color = debug->GetColor();
		if (ImGui::ColorEdit4("Color##Capsule", (float*)&color))
			debug->SetColor(color);
	}
}

void CapsuleCollider::Height(float height)
{
	this->height = height;
	if(bDebug)
		debug->Set(radius, height);
		//debug->Height(height);
}

void CapsuleCollider::Radius(float radius)
{
	this->radius = radius;
	
	if(bDebug)
		debug->Set(radius, height);
		//debug->Radius(radius);
}

void CapsuleCollider::Get(OUT D3DXVECTOR3 * a, OUT D3DXVECTOR3 * b, OUT float * radius)
{
	D3DXVECTOR3 dir;

	switch (axis)
	{
	case CapsuleCollider::Axis::Axis_X: 
		dir = D3DXVECTOR3(1, 0, 0); 
		*radius = this->radius * Scale().y;
		break;
	case CapsuleCollider::Axis::Axis_Y: 
		dir = D3DXVECTOR3(0, 1, 0);	
		*radius = this->radius * Scale().x;
		break;
	case CapsuleCollider::Axis::Axis_Z: 
		dir = D3DXVECTOR3(0, 0, 1); 
		*radius = this->radius * Scale().x;
		break;
	}

	*a = dir * (-height) * 0.5f;
	*b = dir * height * 0.5f;

	D3DXVec3TransformCoord(a, a, &World());
	D3DXVec3TransformCoord(b, b, &World());
}
