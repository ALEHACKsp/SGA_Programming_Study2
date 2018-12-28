#include "stdafx.h"
#include "GameTree.h"

GameTree::GameTree(ExecuteValues * values)
	: GameModel(Materials + L"Meshes/", L"Quad.material", Models + L"Meshes/", L"Quad.mesh")
	, values(values), rotation(0, 0, 0)
{	
	SetShader(Shaders + L"024_Billboard.hlsl");
	SetDiffuseMap(Textures + L"Tree.png");

	cross = new GameModel(Materials + L"Meshes/", L"Quad.material", Models + L"Meshes/", L"Quad.mesh");
	cross->SetShader(Shaders + L"024_Billboard.hlsl");
	cross->SetDiffuseMap(Textures + L"Tree.png");

	cullMode[0] = new RasterizerState();
	cullMode[1] = new RasterizerState();
	cullMode[1]->CullMode(D3D11_CULL_NONE);
}

GameTree::~GameTree()
{
	SAFE_DELETE(cullMode[1]);
	SAFE_DELETE(cullMode[0]);
	SAFE_DELETE(cross);
}

void GameTree::Update()
{
	D3DXMATRIX R;
	values->MainCamera->Matrix(&R);

	D3DXMatrixInverse(&R, NULL, &R);

	float y = atan2(R._31, R._33);

	rotation = D3DXVECTOR3(0, y, 0);

	Rotation(rotation.x, rotation.y + Math::ToRadian(45.0f), rotation.z);
	__super::Update();
	cross->Rotation(rotation.x, rotation.y + Math::ToRadian(-45.0f), rotation.z);
	cross->Position(Position());
	cross->Scale(Scale());
	cross->Update();
}

void GameTree::Render()
{
	cullMode[1]->RSSetState();
	__super::Render();
	cross->Render();
	cullMode[0]->RSSetState();
}
