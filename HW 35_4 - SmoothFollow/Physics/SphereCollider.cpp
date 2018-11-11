#include "stdafx.h"
#include "SphereCollider.h"

SphereCollider::SphereCollider(float radius, bool bDebug)
	: radius(radius), bDebug(bDebug)
{
	type = Type::SPHERE;

	if (bDebug)
		debug = new DebugDrawSphere(radius);
	else
		debug = NULL;
}

SphereCollider::~SphereCollider()
{
	SAFE_DELETE(debug);
}

void SphereCollider::Update(D3DXMATRIX & world)
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

void SphereCollider::Update()
{
	if (bDebug) {
		debug->Scale(Scale());
		debug->Rotation(Rotation());
		debug->Position(Position());
	}
}

void SphereCollider::Render()
{
	if (bDebug)
		debug->Render();
}

void SphereCollider::PostRender()
{
	__super::PostRender();

	D3DXVECTOR3 s, r, t;
	s = Scale();
	r = Rotation();
	t = Position();

	if (ImGui::DragFloat3("Position##Sphere", (float*)&t, 0.05f))
		Position(t);
	if (ImGui::DragFloat3("Rotation##Sphere", (float*)&r, 0.01f))
		Rotation(r);
	if (ImGui::DragFloat3("Scale##Sphere", (float*)&s, 0.1f))
		Scale(s);

	if (ImGui::DragFloat("Radius##Sphere", &radius))
		Radius(radius);

	if (bDebug) {
		D3DXCOLOR color = debug->GetColor();
		if (ImGui::ColorEdit4("Color##Sphere", (float*)&color))
			debug->SetColor(color);
	}
}

void SphereCollider::Radius(float radius)
{
	this->radius = radius;
	if (bDebug)
		debug->Radius(radius);
}
