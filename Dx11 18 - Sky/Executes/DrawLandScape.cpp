#include "stdafx.h"
#include "DrawLandScape.h"

#include "../LandScape/Sky.h"
#include "../LandScape/Terrain.h"
#include "../Objects/MeshSphere.h"

DrawLandScape::DrawLandScape(ExecuteValues * values)
	:Execute(values)
{
	// Create Sky
	{
		sky = new Sky(values);
	}

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
	SAFE_DELETE(sphere);
	SAFE_DELETE(terrain);
	SAFE_DELETE(sky);
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

	// 1.5f ũ�� �÷����� �� ���ذ� �߽��� ������ �ٵ� sphere �ƴ�
	//position.y = terrain->Y(position); 
	
	// �浹 ��� ���
	D3DXVECTOR3 newPosition;
	if (terrain->Y(&newPosition, position) == true)
		position.y = newPosition.y;

	sphere->Position(position);

	sky->Update();
	terrain->Update();
	sphere->Update();
}

void DrawLandScape::PreRender()
{

}

void DrawLandScape::Render()
{
	sky->Render();
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

