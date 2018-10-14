#include "stdafx.h"
#include "TestShadow.h"

#include "../Objects/GameAnimModel.h"

#include "../Objects/MeshPlane.h"
#include "../Objects/MeshSphere.h"

#include "../Model/ModelClip.h"

#include "../Lights/Shadow.h"

TestShadow::TestShadow(ExecuteValues * values)
	:Execute(values)
{
	// model
	{
		hanjo = new GameAnimModel(
			Models + L"Kachujin/", L"Kachujin.material",
			Models + L"Kachujin/", L"Kachujin.mesh");

		hanjoShader = new Shader(Shaders + L"029_Animation.hlsl");
		hanjo->SetShader(hanjoShader);

		hanjo->SetDiffuse(1, 1, 1, 1);
		hanjo->AddClip(Models + L"Kachujin/Samba_Dance.anim");
		hanjo->Play(0, true, 0, 5.0f);

		hanjo->Scale(0.025f, 0.025f, 0.025f);
		//hanjo->Scale(1.0f, 1.0f, 1.0f);
	}


	// mesh
	{
		plane = new MeshPlane();
		planeShader = new Shader(Shaders + L"033_Plane.hlsl");
		plane->SetShader(planeShader);
		plane->SetDiffuse(1, 1, 1, 1);
		plane->Scale(10, 1, 10);
		plane->SetDiffuseMap(Textures + L"White.png");

		sun = new MeshSphere();
		sunShader = new Shader(Shaders + L"Homework/Mesh2.hlsl");
		sun->SetDiffuse(1, 0, 0, 1);
		sun->SetDiffuseMap(Textures + L"Homework/Sun.png");
		sun->SetShader(sunShader);

		board = new MeshPlane();
		texture = new Texture();
		board->SetDiffuseMap(texture);
		board->Scale(1, 1, 1);
		board->Position(-20, 5, 10);
		board->RotationDegree(-90, 0, 0);

		cctv = new MeshPlane();
		texture2 = new Texture();
		cctv->SetShader(sunShader);
		cctv->SetDiffuseMap(texture2);
		cctv->Scale(3, 1, 3);
		cctv->Position(-40, 14.9f, 5);
		cctv->RotationDegree(-90, -60, 0);

		//D3DDesc desc;
		//D3D::GetDesc(&desc);
		//render2D = new Render2D(values);
		//render2D->Scale(desc.Width * 0.25f, desc.Height * 0.25f);
		renderTarget = new RenderTarget(values);

		wall = new MeshPlane();
		wall->Scale(5, 1, 1);
		wall->Position(0, 5, 10);
		wall->RotationDegree(-90, 60, 0);
		wall->SetDiffuse(1, 1, 1, 1);
		wall->SetDiffuseMap(Textures + L"White.png");
	}


	shadow = new Shadow(values);
	shadow->Add(hanjo);
	shadow->Add(plane);
	shadow->Add(wall);

	//shadow->Add(board);
	//shadow->Add(cctv);
}

TestShadow::~TestShadow()
{
	//SAFE_DELETE(render2D);
	SAFE_DELETE(renderTarget);

	SAFE_DELETE(shadow);

	SAFE_DELETE(hanjo);
	SAFE_DELETE(hanjoShader);

	SAFE_DELETE(plane);
	SAFE_DELETE(planeShader);

	SAFE_DELETE(texture);
	SAFE_DELETE(board);

	SAFE_DELETE(texture2);
	SAFE_DELETE(cctv);

	SAFE_DELETE(wall);

	SAFE_DELETE(sun);
	SAFE_DELETE(sunShader);
}

void TestShadow::Update()
{
	sun->Position(values->GlobalLight->Data.Position);
	sun->Update();

	cctv->Update();

	texture->SetView(shadow->GetSRV());
	board->Update();

	shadow->Update();

	//plane->Update();
}

void TestShadow::PreRender()
{
	shadow->PreRender();

	renderTarget->Set();
	{
		//plane->Update();
		//plane->Render();

		sun->Render();

		board->Render();
		//cctv->Render();

		shadow->ObjectsRender();
	}

	cctv->Update();
	texture2->SetView(renderTarget->GetSRV());	
}

void TestShadow::Render()
{
	sun->Render();

	board->Render();
	cctv->Render();

	shadow->Render();
	
	// ImGui
	{
		ImGui::Begin("Board");

		D3DXVECTOR3 position = board->Position();
		if (ImGui::DragFloat3("Board Position", (float*)&position))
			board->Position(position);
		D3DXVECTOR3 rotation = board->RotationDegree();
		if (ImGui::DragFloat3("Board Rotation", (float*)&rotation))
			board->RotationDegree(rotation);
		D3DXVECTOR3 scale = board->Scale();
		if (ImGui::DragFloat3("Board Scale", (float*)&scale))
			board->Scale(scale);

		ImGui::End();

		ImGui::Begin("Wall");

		position = wall->Position();
		if (ImGui::DragFloat3("Wall Position", (float*)&position))
			wall->Position(position);
		rotation = wall->RotationDegree();
		if (ImGui::DragFloat3("Wall Rotation", (float*)&rotation))
			wall->RotationDegree(rotation);
		scale = wall->Scale();
		if (ImGui::DragFloat3("Wall Scale", (float*)&scale))
			wall->Scale(scale);

		ImGui::End();

		//ImGui::Begin("Sun");

		//position = sun->Position();
		//if (ImGui::DragFloat3("Sun Position", (float*)&position))
		//	sun->Position(position);
		//rotation = sun->RotationDegree();
		//if (ImGui::DragFloat3("Sun Rotation", (float*)&rotation))
		//	sun->RotationDegree(rotation);
		//scale = sun->Scale();
		//if (ImGui::DragFloat3("Sun Scale", (float*)&scale))
		//	sun->Scale(scale);

		//ImGui::End();

		ImGui::Begin("Cctv");

		position = cctv->Position();
		if (ImGui::DragFloat3("Cctv Position", (float*)&position))
			cctv->Position(position);
		rotation = cctv->RotationDegree();
		if (ImGui::DragFloat3("Cctv Rotation", (float*)&rotation))
			cctv->RotationDegree(rotation);
		scale = cctv->Scale();
		if (ImGui::DragFloat3("Cctv Scale", (float*)&scale))
			cctv->Scale(scale);

		ImGui::End();

	}

}

void TestShadow::PostRender()
{

}

