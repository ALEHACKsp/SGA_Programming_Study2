#include "stdafx.h"
#include "TestMultiPicking.h"

#include "../Objects/MeshPlane.h"
#include "../Objects/MeshSphere.h"

#include "../Viewer/Fixity.h"
#include "../Viewer/Frustum.h"


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

	pickingCamera = new Fixity();
	//pickingCamera->Position(0, 0, -50);
	pickingProjection = new Perspective(1024, 768);
	pickingFrustum = new Frustum(values, 100, pickingCamera, pickingProjection);
	checkPicking = false;
	startPos = { 0,0,0 };
	endPos = { 0,0,0 };
}

TestMultiPicking::~TestMultiPicking()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(sphere);
	SAFE_DELETE(plane);

	//SAFE_DELETE(camera);

	SAFE_DELETE(frustum);
	//SAFE_DELETE(projection);

	SAFE_DELETE(pickingCamera);
	SAFE_DELETE(pickingFrustum);
	SAFE_DELETE(pickingProjection);
}

void TestMultiPicking::Update()
{
	plane->Update();

	frustum->Update();

	if (Mouse::Get()->Down(0)) {
		startPos = Mouse::Get()->GetPosition();
	}
	if (Mouse::Get()->Up(0)) {
		endPos = Mouse::Get()->GetPosition();

		D3DXVECTOR3 center = (startPos + endPos) / 2;
		//pickingCamera->Position(center.x, center.y, center.z);
		float width = fabs(endPos.x - startPos.x);
		float height = fabs(endPos.y - startPos.y);
		float h = fabs(endPos.y - startPos.y);
		//float width = 1024;
		//float height = 768;
		//float fov = tan(h) * 2;
		//float temp = Math::ToDegree(fov);
		pickingProjection->Set(width, height, Math::PI * 0.25f, 0.1f, 100);

		checkPicking = true;
		pickingFrustum->Update();
	}
}


void TestMultiPicking::Render()
{
	plane->Render();

	drawCount = 0;
	int i = 0;
	for (D3DXVECTOR3 position : positions)
	{
		if (frustum->ContainPoint(position) == true)
		{
			sphere->Position(position);

			if (checkPicking == true) {
				if (pickingFrustum->ContainPoint(position))
				{
					colors[i] = D3DXCOLOR(0, 0, 1, 1);
				}
				else
					colors[i] = D3DXCOLOR(1, 1, 1, 1);
			}

			sphere->SetDiffuse(colors[i]);

			sphere->Update();
			sphere->Render();

			drawCount++;
		}
		i++;
	}

	if (checkPicking)
		checkPicking = false;

	ImGui::Text("Draw %d / %d", drawCount, positions.size());
}

