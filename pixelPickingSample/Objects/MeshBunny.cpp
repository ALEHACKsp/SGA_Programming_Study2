#include "stdafx.h"
#include "MeshBunny.h"

MeshBunny::MeshBunny()
	: GameModel(Materials + L"StanfordBunny/", L"StanfordBunny.material", Models + L"StanfordBunny/", L"StanfordBunny.mesh")
{
}

MeshBunny::~MeshBunny()
{
	SAFE_DELETE(shader)
}

void MeshBunny::Update()
{
	__super::Update();
}

void MeshBunny::Render()
{
	__super::Render();
}
