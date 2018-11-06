#include "stdafx.h"
#include "TestLight.h"

#include "../Objects/MeshPlane.h"
#include "../Objects/MeshSphere.h"

#include "../Viewer/Orbit.h"

TestLight::TestLight(ExecuteValues * values)
	:Execute(values)
	, targetPos(0, 5, 0)
{
	shader = new Shader(Shaders + L"038_Lighting.hlsl");

	// Mesh
	{
		plane = new MeshPlane();
		plane->Scale(100, 1, 100);
		plane->SetShader(shader);
		plane->SetDiffuse(1, 1, 1);

		sphere = new MeshSphere();
		sphere->Position(10, 0, 0);
		sphere->SetShader(shader);
		sphere->SetDiffuse(1, 1, 1);
		sphere->Scale(10, 10, 10);
		bool* orbit = sphere->GetIsRotate();
		*orbit = true;
		float* orbitDist = sphere->GetOrbitDist();
		*orbitDist = 50.0f;
		sphere->SetTarget(targetPos);
	}

	// Light Setting
	{
		lightBuffer = new LightBuffer();

		LightData data;
		{
			LightDesc desc;
			desc.Type = ELightType::Spot;
			desc.Position = D3DXVECTOR3(50, 0, 0);
			desc.Direction = D3DXVECTOR3(1, 0, 0);
			desc.Range = 50;
			//desc.Length = 80;
			desc.Outer = 65.0f;
			desc.Inner = 45.0f;
			desc.Color = D3DXVECTOR3(1, 0, 0);
			data.Setting(0, desc);
			lightDescs[0].push_back(desc);

			desc.Type = ELightType::Point;
			desc.Position = D3DXVECTOR3(0, 0, 0);
			//desc.Direction = D3DXVECTOR3(-1, 0, 0);
			desc.Range = 50;
			//desc.Length = 80;
			//desc.Outer = Math::ToRadian(65.0f);
			//desc.Inner = Math::ToRadian(45.0f);
			desc.Color = D3DXVECTOR3(1, 1, 1);
			data.Setting(1, desc);
			lightDescs[0].push_back(desc);

			//desc.Type = ELightType::Spot;
			//desc.Position = D3DXVECTOR3(0, 10, 0);
			//desc.Direction = D3DXVECTOR3(0, 0, 1);
			//desc.Range = 50;
			////desc.Length = 80;
			//desc.Outer = Math::ToRadian(65.0f);
			//desc.Inner = Math::ToRadian(45.0f);
			//desc.Color = D3DXVECTOR3(0, 0, 1);
			//data.Setting(2, desc);

			//desc.Type = ELightType::Spot;
			//desc.Position = D3DXVECTOR3(0, 10, 0);
			//desc.Direction = D3DXVECTOR3(0, 0, -1);
			//desc.Range = 50;
			//desc.Length = 80;
			//desc.Outer = Math::ToRadian(65.0f);
			//desc.Inner = Math::ToRadian(45.0f);
			//desc.Color = D3DXVECTOR3(1, 1, 0);
			//data.Setting(3, desc);
		}
		lightBuffer->AddLight(data);

		{
			LightDesc desc;
			desc.Type = ELightType::Spot;
			desc.Position = D3DXVECTOR3(0, 0, 0);
			desc.Direction = D3DXVECTOR3(-1, 0, 0);
			desc.Range = 50;
			//desc.Length = 80;
			desc.Outer = 65.0f;
			desc.Inner = 45.0f;
			desc.Color = D3DXVECTOR3(0, 1, 0);
			data.Setting(0, desc);
			lightDescs[1].push_back(desc);

			desc.Type = ELightType::Capsule;
			desc.Position = D3DXVECTOR3(0, 10, 0);
			desc.Direction = D3DXVECTOR3(-1, 0, 0);
			desc.Range = 50;
			desc.Length = 80;
			//desc.Outer = Math::ToRadian(65.0f);
			//desc.Inner = Math::ToRadian(45.0f);
			desc.Color = D3DXVECTOR3(0, 1, 1);
			data.Setting(1, desc);
			lightDescs[1].push_back(desc);

		//	//desc.Type = ELightType::Spot;
		//	//desc.Position = D3DXVECTOR3(0, 10, 0);
		//	//desc.Direction = D3DXVECTOR3(0, 0, 1);
		//	//desc.Range = 50;
		//	////desc.Length = 80;
		//	//desc.Outer = Math::ToRadian(65.0f);
		//	//desc.Inner = Math::ToRadian(45.0f);
		//	//desc.Color = D3DXVECTOR3(0, 0, 1);
		//	//data.Setting(2, desc);

		//	//desc.Type = ELightType::Spot;
		//	//desc.Position = D3DXVECTOR3(0, 10, 0);
		//	//desc.Direction = D3DXVECTOR3(0, 0, -1);
		//	//desc.Range = 50;
		//	//desc.Length = 80;
		//	//desc.Outer = Math::ToRadian(65.0f);
		//	//desc.Inner = Math::ToRadian(45.0f);
		//	//desc.Color = D3DXVECTOR3(1, 1, 0);
		//	//data.Setting(3, desc);
		}
		lightBuffer->AddLight(data);

		{
			LightDesc desc;
			desc.Type = ELightType::Spot;
			desc.Position = D3DXVECTOR3(0, 0, 50);
			desc.Direction = D3DXVECTOR3(-1, 0, 0);
			desc.Range = 50;
			//desc.Length = 80;
			desc.Outer = 65.0f;
			desc.Inner = 45.0f;
			desc.Color = D3DXVECTOR3(0, 0, 1);
			data.Setting(0, desc);
			lightDescs[2].push_back(desc);

			//	//desc.Type = ELightType::Spot;
			//	//desc.Position = D3DXVECTOR3(0, 10, 0);
			//	//desc.Direction = D3DXVECTOR3(-1, 0, 0);
			//	//desc.Range = 50;
			//	////desc.Length = 80;
			//	//desc.Outer = Math::ToRadian(65.0f);
			//	//desc.Inner = Math::ToRadian(45.0f);
			//	//desc.Color = D3DXVECTOR3(0, 1, 0);
			//	//data.Setting(1, desc);

			//	//desc.Type = ELightType::Spot;
			//	//desc.Position = D3DXVECTOR3(0, 10, 0);
			//	//desc.Direction = D3DXVECTOR3(0, 0, 1);
			//	//desc.Range = 50;
			//	////desc.Length = 80;
			//	//desc.Outer = Math::ToRadian(65.0f);
			//	//desc.Inner = Math::ToRadian(45.0f);
			//	//desc.Color = D3DXVECTOR3(0, 0, 1);
			//	//data.Setting(2, desc);

			//	//desc.Type = ELightType::Spot;
			//	//desc.Position = D3DXVECTOR3(0, 10, 0);
			//	//desc.Direction = D3DXVECTOR3(0, 0, -1);
			//	//desc.Range = 50;
			//	//desc.Length = 80;
			//	//desc.Outer = Math::ToRadian(65.0f);
			//	//desc.Inner = Math::ToRadian(45.0f);
			//	//desc.Color = D3DXVECTOR3(1, 1, 0);
			//	//data.Setting(3, desc);
		}
		lightBuffer->AddLight(data);

		{
			LightDesc desc;
			desc.Type = ELightType::Spot;
			desc.Position = D3DXVECTOR3(0, 0, -50);
			desc.Direction = D3DXVECTOR3(-1, 0, 0);
			desc.Range = 50;
			//desc.Length = 80;
			desc.Outer = 65.0f;
			desc.Inner = 45.0f;
			desc.Color = D3DXVECTOR3(1, 1, 0);
			data.Setting(0, desc);
			lightDescs[3].push_back(desc);

			//	//desc.Type = ELightType::Spot;
			//	//desc.Position = D3DXVECTOR3(0, 10, 0);
			//	//desc.Direction = D3DXVECTOR3(-1, 0, 0);
			//	//desc.Range = 50;
			//	////desc.Length = 80;
			//	//desc.Outer = Math::ToRadian(65.0f);
			//	//desc.Inner = Math::ToRadian(45.0f);
			//	//desc.Color = D3DXVECTOR3(0, 1, 0);
			//	//data.Setting(1, desc);

			//	//desc.Type = ELightType::Spot;
			//	//desc.Position = D3DXVECTOR3(0, 10, 0);
			//	//desc.Direction = D3DXVECTOR3(0, 0, 1);
			//	//desc.Range = 50;
			//	////desc.Length = 80;
			//	//desc.Outer = Math::ToRadian(65.0f);
			//	//desc.Inner = Math::ToRadian(45.0f);
			//	//desc.Color = D3DXVECTOR3(0, 0, 1);
			//	//data.Setting(2, desc);

			//	//desc.Type = ELightType::Spot;
			//	//desc.Position = D3DXVECTOR3(0, 10, 0);
			//	//desc.Direction = D3DXVECTOR3(0, 0, -1);
			//	//desc.Range = 50;
			//	//desc.Length = 80;
			//	//desc.Outer = Math::ToRadian(65.0f);
			//	//desc.Inner = Math::ToRadian(45.0f);
			//	//desc.Color = D3DXVECTOR3(1, 1, 0);
			//	//data.Setting(3, desc);
		}
		lightBuffer->AddLight(data);

		//LightData data;
		//{
		//	LightDesc desc;
		//	desc.Type = ELightType::None;
		//	desc.Position = D3DXVECTOR3(0, 10, 0);
		//	//desc.Direction = D3DXVECTOR3(1, 0, 0);
		//	desc.Range = 50;
		//	//desc.Length = 80;
		//	desc.Color = D3DXVECTOR3(1, 0, 0);
		//	data.Setting(0, desc);
		//
		//	desc.Type = ELightType::Point;
		//	desc.Position = D3DXVECTOR3(0, 10, 0);
		//	//desc.Direction = D3DXVECTOR3(-1, 0, 0);
		//	desc.Range = 50;
		//	//desc.Length = 80;
		//	desc.Color = D3DXVECTOR3(0, 1, 0);
		//	data.Setting(1, desc);
		//
		//	desc.Type = ELightType::Spot;
		//	desc.Position = D3DXVECTOR3(0, 10, 0);
		//	desc.Direction = D3DXVECTOR3(0, 0, 1);
		//	desc.Range = 50;
		//	desc.Outer = Math::ToRadian(65.0f);
		//	desc.Inner = Math::ToRadian(45.0f);
		//	//desc.Length = 80;
		//	desc.Color = D3DXVECTOR3(0, 0, 1);
		//	data.Setting(2, desc);
		//
		//	desc.Type = ELightType::Capsule;
		//	desc.Position = D3DXVECTOR3(0, 10, 0);
		//	desc.Direction = D3DXVECTOR3(0, 0, -1);
		//	desc.Range = 50;
		//	desc.Length = 80;
		//	desc.Color = D3DXVECTOR3(1, 1, 0);
		//	data.Setting(3, desc);
		//	lightCount++;
		//}
		//lightBuffer->AddLight(data);
	}
	
	orbit = new Orbit();
	orbit->SetOrbitDist(250.0f);

	values->MainCamera = orbit;
	values->MainCamera->RotationDegree(45.0f, 0);
}

TestLight::~TestLight()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(lightBuffer);

	SAFE_DELETE(plane);
	SAFE_DELETE(sphere);

	SAFE_DELETE(orbit);
}

void TestLight::Update()
{
	plane->Update();
	sphere->Update();

	orbit->SetTarget(plane->Position());
}

void TestLight::PreRender()
{

}

void TestLight::Render()
{
	ImGui::Checkbox("IsOrbit", sphere->GetIsRotate());
	ImGui::DragFloat("Distance", sphere->GetOrbitDist());
	if (ImGui::DragFloat3("TargetPos", (float*)&targetPos, 0.1f))
		sphere->SetTarget(targetPos);

	SettingLightData();

	lightBuffer->SetPSBuffer(2);

	plane->Render();
	sphere->Render();
}

void TestLight::PostRender()
{

}

void TestLight::SettingLightData()
{
	LightData data;

	ImGui::Begin("Light");

	for (int j = 0; j < lightBuffer->GetCount(); j++) {
		ImGui::Text(("Light's " + to_string(j)).c_str());

		for (int i = 0; i < lightDescs[j].size(); i++) {
			LightDesc desc = lightDescs[j][i];
			if (desc.Type == ELightType::None) continue;

			string id = "##" + to_string(j) + "_" + to_string(i);

			switch (desc.Type)
			{
			case TestLight::ELightType::None:
			{
				ImGui::Text("Type : None");
				ImGui::DragFloat3(("Pos" + id).c_str(), (float*)&desc.Position, 0.1f);
				ImGui::DragFloat(("Range" + id).c_str(), &desc.Range);
				ImGui::ColorEdit3(("Color" + id).c_str(), (float*)&desc.Color);
			}
			break;
			case TestLight::ELightType::Point:
			{
				ImGui::Text("Type : Point");
				ImGui::DragFloat3(("Pos" + id).c_str(), (float*)&desc.Position, 0.1f);
				ImGui::DragFloat(("Range" + id).c_str(), &desc.Range);
				ImGui::ColorEdit3(("Color" + id).c_str(), (float*)&desc.Color);
			}
			break;
			case TestLight::ELightType::Spot:
			{
				ImGui::Text("Type : Spot");
				ImGui::DragFloat3(("Pos" + id).c_str(), (float*)&desc.Position, 0.1f);
				ImGui::DragFloat3(("Dir" + id).c_str(), (float*)&desc.Direction, 0.1f);
				ImGui::DragFloat(("Range" + id).c_str(), &desc.Range);
				ImGui::DragFloat(("Outer" + id).c_str(), &desc.Outer);
				ImGui::DragFloat(("Inner" + id).c_str(), &desc.Inner);
				ImGui::ColorEdit3(("Color" + id).c_str(), (float*)&desc.Color);
			}
			break;
			case TestLight::ELightType::Capsule:
			{
				ImGui::Text("Type : Capsule");
				ImGui::DragFloat3(("Pos" + id).c_str(), (float*)&desc.Position, 0.1f);
				ImGui::DragFloat3(("Dir" + id).c_str(), (float*)&desc.Direction, 0.1f);
				ImGui::DragFloat(("Range" + id).c_str(), &desc.Range);
				ImGui::DragFloat(("Length" + id).c_str(), &desc.Length);
				ImGui::ColorEdit3(("Color" + id).c_str(), (float*)&desc.Color);
			}
			break;
			}

			ImGui::Separator();
			lightDescs[j][i] = desc;
			data.Setting(i, desc);
		}

		lightBuffer->SetLight(data, j);
	}

	ImGui::End();
}



