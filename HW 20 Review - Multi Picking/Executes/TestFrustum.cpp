#include "stdafx.h"
#include "TestFrustum.h"

#include "../Objects/MeshPlane.h"
#include "../Objects/MeshSphere.h"

#include "../Viewer/Fixity.h"
#include "../Viewer/Frustum.h"


TestFrustum::TestFrustum(ExecuteValues * values)
	:Execute(values), drawCount(0)
	//, w(1024), h(768)
	//, zFar(100)
	, fov(45.0f)
{
	shader = new Shader(Shaders + L"022_Diffuse.hlsl");

	plane = new MeshPlane();
	plane->Scale(10, 10, 10);
	plane->SetShader(shader);
	plane->SetDiffuse(1, 0, 0, 1);

	sphere = new MeshSphere();
	sphere->SetShader(shader);
	sphere->SetDiffuse(1, 1, 1, 1);

	camera = new Fixity();
	camera->Position(0, 0, -50);

	projection = new Perspective(1024, 768, Math::PI * 0.25f, 1, 1000);

	//frustum = new Frustum(values, 100, camera, projection);
	frustum = new Frustum(values, 100);

	for (float z = -50; z < 50; z += 10)
	{
		for (float y = -50; y < 50; y += 10)
		{
			for (float x = -50; x < 50; x += 10)
			{
				positions.push_back(D3DXVECTOR3(x, y, z));
			}
		}
	}
}

TestFrustum::~TestFrustum()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(sphere);
	SAFE_DELETE(plane);

	SAFE_DELETE(camera);

	SAFE_DELETE(frustum);
	SAFE_DELETE(projection);
}

void TestFrustum::Update()
{
	plane->Update();

	frustum->Update();
}


void TestFrustum::Render()
{
	//plane->Render();

	drawCount = 0;
	for (D3DXVECTOR3 position : positions)
	{
		if (frustum->ContainPoint(position) == true)
		{
			sphere->Position(position);
			sphere->Update();
			sphere->Render();

			drawCount++;
		}
	}

	ImGui::Text("Draw %d / %d", drawCount, positions.size());
	//ImGui::InputFloat("w", &w);
	//ImGui::InputFloat("h", &h);
	//ImGui::InputFloat("zFar", &zFar);
	//frustum->SetzFar(zFar);
	//projection->SetWH(w, h);
	ImGui::InputFloat("Fov", &fov);
	projection->Set(1024, 768, Math::ToRadian(fov), 1, 1000);
}

