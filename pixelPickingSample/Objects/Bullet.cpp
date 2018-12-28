#include "stdafx.h"
#include "Bullet.h"

Bullet::Bullet(float destroyTime)
	: GameModel(Materials + L"Meshes/", L"Sphere.material", Models + L"Meshes/", L"Sphere.mesh")
	, destroyTime(destroyTime), isDestroy(false)
{
	shader = new Shader(Shaders + L"017_Sphere.hlsl");
	for (Material* material : model->Materials())
	{
		material->SetDiffuse(D3DXCOLOR(1, 0, 0, 1));
		material->SetShader(shader);
	}

	fireTime = Time::Get()->Running();
}

Bullet::~Bullet()
{
	SAFE_DELETE(shader)
}

void Bullet::Update()
{
	__super::Update();

	if (Time::Get()->Running() - fireTime > destroyTime)
		isDestroy = true;
}

void Bullet::Render()
{
	__super::Render();
}
