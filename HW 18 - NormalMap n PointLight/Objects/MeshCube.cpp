#include "stdafx.h"
#include "MeshCube.h"

MeshCube::MeshCube()
	: GameModel(Materials + L"Meshes/", L"Cube.material", 
		Models + L"Meshes/", L"Cube.mesh" )
{
	shader = new Shader(Shaders + L"Homework/NormalMap.hlsl");
	this->SetShader(shader);

	//SetDiffuseMap(Textures + L"Wall.png");
	//SetSpecularMap(Textures + L"Wall_specular.png");
	//SetNormalMap(Textures + L"Wall_normal.png");

	name = "Cube";

	diffuseColor = D3DXCOLOR(1, 1, 1, 1);
	specularColor = D3DXCOLOR(1, 1, 1, 1);
	shininess = 15.0f;

	SetDiffuse(diffuseColor);
	SetSpecular(specularColor);
	SetShininess(shininess);

	val = 0;
}

MeshCube::~MeshCube()
{

}

void MeshCube::Update()
{
	//GameModel::Update();
	// 이렇게 써도 됨 바로위에 부모 virtual 없으면 더 위로
	__super::Update(); 
}

void MeshCube::Render()
{
	__super::Render();
}

void MeshCube::PostRender()
{
	__super::PostRender();

	//ImGui::Spacing();
	ImGui::Separator();
	ImGui::Text("Material");

	ImGui::ColorEdit4("Diffuse", (float*)&diffuseColor);
	ImGui::ColorEdit4("Specular", (float*)&specularColor);
	ImGui::SliderFloat("Shininess", &shininess, 0.1f, 30.0f);

	SetDiffuse(diffuseColor);
	SetSpecular(specularColor);
	SetShininess(shininess);
}
