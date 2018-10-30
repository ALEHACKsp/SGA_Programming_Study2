#include "stdafx.h"
#include "SphereCollider.h"

SphereCollider::SphereCollider(float radius)
	: radius(radius)
{
	type = Type::SPHERE;

	debug = new DebugDrawSphere(radius);
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

	debug->Scale(S);
	debug->Rotation(rotation);
	debug->Position(T);
}

void SphereCollider::Render()
{
	debug->Render();
}

void SphereCollider::PostRender()
{
	__super::PostRender();

	if (ImGui::DragFloat("Radius", &radius))
		Radius(radius);
}

void SphereCollider::Radius(float radius)
{
	this->radius = radius;
	debug->Set(radius);
}
