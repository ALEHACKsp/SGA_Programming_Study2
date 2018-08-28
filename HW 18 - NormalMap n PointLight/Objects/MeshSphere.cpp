#include "stdafx.h"
#include "MeshSphere.h"

MeshSphere::MeshSphere()
	: GameModel(Materials + L"Meshes/", L"Sphere.material", 
		Models + L"Meshes/", L"Sphere.mesh" )
{
	shader = new Shader(Shaders + L"018_Specular.hlsl");
	this->SetShader(shader);

	SetDiffuseMap(Textures + L"Wall.png");
	//SetSpecularMap(Textures + L"Wall_specular.png");

	name = "Sphere";

	diffuseColor = D3DXCOLOR(1, 1, 1, 1);
	specularColor = D3DXCOLOR(1, 1, 1, 1);
	shininess = 15.0f;

	SetDiffuse(diffuseColor);
	SetSpecular(specularColor);
	SetShininess(shininess);

	val = 0;
}

MeshSphere::~MeshSphere()
{

}

void MeshSphere::Update()
{
	//GameModel::Update();
	// 이렇게 써도 됨 바로위에 부모 virtual 없으면 더 위로
	__super::Update(); 

	// 자전
	{
		val += rotateSpeed * Time::Delta();
		if (val > D3DX_PI * 2)
			val = 0;

		D3DXVECTOR3 R = Rotation();
		R.y = val;

		Rotation(R);

		//D3DXVECTOR3 rotation = Rotation();
		//rotation.x += rotateSpeed.x * Time::Delta();
		//if (rotation.x > D3DX_PI * 2)
		//	rotation.x = 0;
		//rotation.y += rotateSpeed.y * Time::Delta();
		//if (rotation.y > D3DX_PI * 2)
		//	rotation.y = 0;
		//rotation.z += rotateSpeed.z * Time::Delta();
		//if (rotation.z > D3DX_PI * 2)
		//	rotation.z = 0;

		//Rotation(rotation);
	}
	// 공전
	if(isOrbit)
	{
		D3DXMATRIX matT, matR, matTargetT;
		D3DXVECTOR3 rotation;
		D3DXVECTOR3 finalPos = { 0, 0, 0 };
		D3DXMATRIX matFinal;

		D3DXMatrixIdentity(&matT);
		D3DXMatrixIdentity(&matTargetT);

		D3DXMatrixTranslation(&matT, 0, 0, -orbitDist);
		//D3DXMatrixTranslation(&matT, -orbitDist.x, -orbitDist.y, -orbitDist.z);
		D3DXMatrixTranslation(&matTargetT,
			targetPos.x, targetPos.y, targetPos.z);
		rotation = Rotation();
		D3DXMatrixRotationYawPitchRoll(&matR, rotation.y, rotation.x, rotation.z);

		matFinal = matT * matR;
		D3DXVec3TransformCoord(&finalPos, &finalPos, &matFinal);
		D3DXVec3TransformCoord(&finalPos, &finalPos, &matTargetT);

		Position(finalPos.x, finalPos.y, finalPos.z);
	}
}

void MeshSphere::Render()
{
	__super::Render();
}

void MeshSphere::PostRender()
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
