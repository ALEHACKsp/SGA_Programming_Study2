#include "stdafx.h"
#include "TestMultiPicking.h"

#include "../Objects/MeshPlane.h"
#include "../Objects/MeshSphere.h"

#include "../Viewer/Fixity.h"
#include "../Viewer/Frustum.h"
#include "../Viewer/PickingFrustum.h"


TestMultiPicking::TestMultiPicking(ExecuteValues * values)
	:Execute(values), drawCount(0)
{
	shader = new Shader(Shaders + L"022_Diffuse.hlsl");

	plane = new MeshPlane();
	plane->Scale(10, 10, 10);
	plane->SetShader(shader);
	plane->SetDiffuse(1, 0, 0, 1);

	sphere = new MeshSphere();
	sphere->SetShader(shader);
	sphere->SetDiffuse(1, 1, 1, 1);

	//camera = new Fixity();
	//camera->Position(0, 0, -50);

	//projection = new Perspective(1024, 768, Math::PI * 0.25f, 1, 1000);

	//frustum = new Frustum(values, 100, camera, projection);
	frustum = new Frustum(values, 100);

	for (float z = -50; z < 50; z += 10)
	{
		for (float y = -50; y < 50; y += 10)
		{
			for (float x = -50; x < 50; x += 10)
			{
				positions.push_back(D3DXVECTOR3(x, y, z));
				colors.push_back(D3DXCOLOR(1, 1, 1, 1));
			}
		}
	}


	pickingFrustum = new PickingFrustum(values, 100);
}

TestMultiPicking::~TestMultiPicking()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(sphere);
	SAFE_DELETE(plane);

	//SAFE_DELETE(camera);

	SAFE_DELETE(frustum);
	//SAFE_DELETE(projection);

	SAFE_DELETE(pickingFrustum);
}

void TestMultiPicking::Update()
{
	plane->Update();

	frustum->Update();

	pickingFrustum->Update();
}


void TestMultiPicking::Render()
{
	// Line 맨 먼저 그릴거면
	// Line Class 가 너무 옛날 Shader써서 
	// 0번의 Direction 들어간걸 Color로 바꿔서 문제가 있었음 다시 넣어줘야함
	//values->GlobalLight->SetPSBuffer(0);

	plane->Render();

	drawCount = 0;
	int i = 0;
	for (D3DXVECTOR3 position : positions)
	{
		if (frustum->ContainPoint(position) == true)
		{
			sphere->Position(position);

			if (pickingFrustum->ContainPoint(position))
				colors[i] = D3DXCOLOR(0, 0, 1, 1);
			else
				colors[i] = D3DXCOLOR(1, 1, 1, 1);

			sphere->SetDiffuse(colors[i]);

			sphere->Update();
			sphere->Render();

			drawCount++;
		}
		i++;
	}

	pickingFrustum->Render();

	ImGui::Text("Draw %d / %d", drawCount, positions.size());
}

