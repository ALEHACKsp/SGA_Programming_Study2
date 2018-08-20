#include "stdafx.h"
#include "DrawLandScape.h"

#include "../LandScape/Terrain.h"
#include "../Objects/MeshSphere.h"

DrawLandScape::DrawLandScape(ExecuteValues * values)
	:Execute(values)
{
	// Create Terrain
	{
		wstring heightMap = Contents + L"HeightMaps/HeightMap256.png";
		Material * material = new Material(Shaders + L"014_TerrainSplatting.hlsl");

		terrain = new Terrain(values, material);
	}

	// Create MeshSphere
	{
		sphere = new MeshSphere();

		D3DXMATRIX S;
		D3DXMatrixScaling(&S, 3, 3, 3);
		sphere->RootAxis(S);

		sphere->Position(D3DXVECTOR3(80, 10, 30));
	}
}

DrawLandScape::~DrawLandScape()
{
	SAFE_DELETE(terrain);
	SAFE_DELETE(sphere);
}

void DrawLandScape::Update()
{
	D3DXVECTOR3 position = sphere->Position();

	if (Keyboard::Get()->Press('I'))
		position.z += 10.0f * Time::Delta();
	else if (Keyboard::Get()->Press('K'))
		position.z -= 10.0f * Time::Delta();

	if (Keyboard::Get()->Press('J'))
		position.x -= 10.0f * Time::Delta();
	else if (Keyboard::Get()->Press('L'))
		position.x += 10.0f * Time::Delta();

	// 1.5f 크기 늘려놔서 더 해준거 중심이 가운데라면 근데 sphere 아님
	//position.y = terrain->Y(position); 
	
	// 충돌 계산 방법
	D3DXVECTOR3 newPosition;
	if (terrain->Y(&newPosition, position) == true)
		position.y = newPosition.y;

	sphere->Position(position);

	terrain->Update();
	sphere->Update();
}

void DrawLandScape::PreRender()
{

}

void DrawLandScape::Render()
{
	terrain->Render();
	sphere->Render();
}

void DrawLandScape::PostRender()
{
	//D3DXVECTOR3 picked;
	//terrain->Y(&picked);

	//ImGui::LabelText("Picked", "%.2f, %.2f, %.2f", 
	//	picked.x, picked.y, picked.z);
}

void DrawLandScape::ResizeScreen()
{
}

