#include "stdafx.h"
#include "MeshQuad.h"

MeshQuad::MeshQuad()
	: GameModel(Models + L"Meshes/", L"Quad.material", 
		Models + L"Meshes/", L"Quad.mesh" )
{
	rasterizerState[0] = new RasterizerState();
	rasterizerState[1] = new RasterizerState();
	rasterizerState[1]->FrontCounterClockwise(true);

}

MeshQuad::~MeshQuad()
{
	SAFE_DELETE(rasterizerState[0]);
	SAFE_DELETE(rasterizerState[1]);
}

void MeshQuad::Update()
{
	//GameModel::Update();
	// 이렇게 써도 됨 바로위에 부모 virtual 없으면 더 위로
	__super::Update(); 
}

void MeshQuad::Render()
{
	rasterizerState[1]->RSSetState();
	__super::Render();
	rasterizerState[0]->RSSetState();
}
