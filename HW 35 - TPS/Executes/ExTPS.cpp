#include "stdafx.h"
#include "ExTPS.h"

#include "../Objects/GameAnimModel.h"
#include "../Objects/Player.h"
#include "../Objects/Monster.h"
#include "../Model/ModelClip.h"

#include "../Renders/Lines/DebugDrawSphere2.h"

#include "./Objects/MeshPlane.h"
#include "./Objects/MeshSphere.h"

#include "./Viewer/SmoothFollow.h"

#include "./LandScape/Sky.h"

ExTPS::ExTPS(ExecuteValues * values)
	:Execute(values)
{
	shader = new Shader(Shaders + L"Homework/Animation.hlsl");

	visibleBuffer = new VisibleBuffer();

	// Create Model
	{
		// Monster
		{
			models.push_back(new Monster(
				Models + L"CastleGuard/", L"CastleGuard.material",
				Models + L"CastleGuard/", L"CastleGuard.mesh"));
			models.back()->SetShader(shader);
			models.back()->Name("CastleGuard");
			models.back()->Scale(D3DXVECTOR3(0.2f, 0.2f, 0.2f));
			models.back()->Position(D3DXVECTOR3(0, 0, 65));

			// Animation
			vector<string> aniFile = { "Idle.anim", "Idle.anim", "Idle.anim", "Hitted.anim" };
			for (int i = 0; i < aniFile.size(); i++) {
				wstring file = Models + String::ToWString(models.back()->Name() + "/" + aniFile[i]);
				models.back()->AddClip(file);
			}
			models.back()->Play(0, true, 0, 5);
		}

		// Player
		{
			models.push_back(new Player(
				Models + L"BigVegas/", L"BigVegas2.material",
				Models + L"BigVegas/", L"BigVegas2.mesh"));
			models.back()->SetShader(shader);
			models.back()->Name("BigVegas");
			models.back()->Scale(D3DXVECTOR3(0.2f, 0.2f, 0.2f));

			// Animation
			vector<string> aniFile = { "Idle.anim", "Walking.anim", "Attack.anim" };
			for (int i = 0; i < aniFile.size(); i++) {
				wstring file = Models + String::ToWString(models.back()->Name() + "/" + aniFile[i]);
				models.back()->AddClip(file);
			}
			models.back()->Play(0, true, 0, 5);
		}

	}

	selectModel = 0;

	planeShader = new Shader(Shaders + L"Homework/Mesh2.hlsl");
	plane = new MeshPlane();
	plane->SetShader(planeShader);
	plane->SetDiffuseMap(Textures + L"Floor.png");
	plane->Scale(50, 1, 50);

	sphere = new MeshSphere();
	sphere->SetShader(planeShader);
	sphere->SetDiffuseMap(Textures + L"Homework/Sun.png");
	sphere->Scale(2, 2, 2);

	smoothFollow = new SmoothFollow(100, 30);
	free = values->MainCamera;
	values->MainCamera = smoothFollow;
	//values->MainCamera = smoothFollow;

	smoothFollow->SetTarget(
		models.back()->Position(), models.back()->Rotation(), models.back()->Up());

	sky = new Sky(values);

	D3DXVECTOR3 pos, dir;
	values->MainCamera->Position(&pos);
	values->MainCamera->Forward(&dir);
	playerRay = new Ray(pos, dir);
	rayLife = 0;


	capsule = new CapsuleCollider();
	capsule->Scale(10, 10, 10);
	capsule->Position(-30, 20, 100);
	bCapsuleCollision = false;
}

ExTPS::~ExTPS()
{
	SAFE_DELETE(sky);

	for (GameAnimModel* model : models)
		SAFE_DELETE(model);

	SAFE_DELETE(shader);

	SAFE_DELETE(visibleBuffer);

	SAFE_DELETE(planeShader);
	SAFE_DELETE(plane);
	SAFE_DELETE(sphere);

	if (values->MainCamera != free)
		values->MainCamera = free;
	SAFE_DELETE(smoothFollow);

	SAFE_DELETE(playerRay);

	SAFE_DELETE(capsule);
}

void ExTPS::Update()
{
	sky->Update();

	//D3DXVECTOR3 forward = models.back()->Direction();
	//D3DXVec3Normalize(&forward, &forward);

	//values->GlobalLight->Data.Direction = D3DXVECTOR3(-forward.z, 0, -forward.z);

	plane->Update();

	for (GameAnimModel* model : models)
		model->Update();

	D3DXVECTOR3 spherePos = models.back()->BonePosition(models.back()->GetModel()->BoneByName(L"Weapon")->Index());
	sphere->Position(spherePos);
	sphere->Update();

	D3DXVECTOR3 pos = models.back()->BonePosition(models.back()->GetModel()->BoneByName(L"newVegas:Head")->Index());

	smoothFollow->SetTarget(
		pos, models.back()->Rotation(), models.back()->Up());


	if (Keyboard::Get()->Press('Z')) {
		if (smoothFollow->GetDistance() >= 30)
			smoothFollow->SetDistance(smoothFollow->GetDistance() - 1);
	}
	if (Keyboard::Get()->Press('X')) {
		smoothFollow->SetDistance(smoothFollow->GetDistance() + 1);
	}

	if (Mouse::Get()->Down(0)) {
		//smoothFollow->Position(&rayPos);
		rayPos = models.back()->BonePosition(models.back()->GetModel()->BoneByName(L"newVegas:Head")->Index());

		//rayDir = smoothFollow->Direction(values->Viewport, values->Perspective);
		D3DXVECTOR3 forward;
		smoothFollow->Forward(&forward);
		rayDir = -models.back()->Direction();
		D3DXVec3Normalize(&rayDir, &rayDir);
		rayDir += forward;
		playerRay->Set(rayPos, rayDir);
		playerRay->SetDebug(true);
		rayLife = 0;

		if (Collision::IsOverlayRayCapsule(playerRay, models[0]->GetCollider())) {
			models[0]->SetState(GameAnimModel::State::Hitted);
			models[0]->GetCollider()->Color(D3DXCOLOR(1, 0, 0, 1));
		}
		else
			models[0]->GetCollider()->Color(D3DXCOLOR(1, 1, 1, 1));
	}

	if (playerRay->GetDebug() == true) {
		rayLife += Time::Delta();

		if (rayLife > 30.0f) {
			playerRay->SetDebug(false);
		}
	}

	if (Keyboard::Get()->Down('C')) ShowCursor(true);
	if (Keyboard::Get()->Up('C')) ShowCursor(false);


	if (!Keyboard::Get()->Press('C'))
	{
		D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();
		float height = smoothFollow->GetHeight();
		// 좌우회전이 y축 회전
		height += val.y * 50 * Time::Delta();
		if (height >= 100) height = 100;
		if (height <= -20) height = -20;
		smoothFollow->SetHeight(height);
	}

	capsule->Update();
	bCapsuleCollision = Collision::IsOverlayRayCapsule(playerRay, capsule);
	if (bCapsuleCollision) capsule->Color(D3DXCOLOR(1, 0, 0, 1));
	else capsule->Color(D3DXCOLOR(1, 1, 1, 1));

}

void ExTPS::PreRender()
{

}

void ExTPS::Render()
{
	sky->Render();

	plane->Render();
	sphere->Render();

	playerRay->Render();

	visibleBuffer->SetPSBuffer(10);
	for (GameAnimModel* model : models) {
		values->GlobalLight->SetPSBuffer(0);
		model->Render();
	}

	capsule->Render();
}

void ExTPS::PostRender()
{
	ImGui::Begin("Ray");
	playerRay->PostRender();
	ImGui::End();

	ImGui::RadioButton("Player", &selectModel, 0);
	//ImGui::SameLine(120);
	//ImGui::RadioButton("Monster", &selectModel, 1);

	models[selectModel]->PostRender();

	smoothFollow->PostRender();

	ImGui::Begin("Capsule Collider");
	ImGui::Text("Ray Capsule Collision %s", bCapsuleCollision ? "True" : "False");
	capsule->PostRender();
	ImGui::End();
	
}
