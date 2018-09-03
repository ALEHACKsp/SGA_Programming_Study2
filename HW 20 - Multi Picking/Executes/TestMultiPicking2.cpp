#include "stdafx.h"
#include "TestMultiPicking2.h"

#include "../Objects/MeshPlane.h"
#include "../Objects/MeshSphere.h"

#include "../Viewer/Fixity.h"
#include "../Viewer/Frustum.h"

#include "../Renders/Lines/Line.h"

TestMultiPicking2::TestMultiPicking2(ExecuteValues * values)
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

	checkPicking = false;
	startPos = { 0,0,0 };
	endPos = { 0,0,0 };
	for (int i = 0; i < 5; i++)
		lines[i] = D3DXVECTOR3(0, 0, i);

	drawRect = false;
	mouseRect = new Line();
	mouseRect->Init(lines, 4, D3DXCOLOR(1, 1, 0, 1), D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
}

TestMultiPicking2::~TestMultiPicking2()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(sphere);
	SAFE_DELETE(plane);

	//SAFE_DELETE(camera);

	SAFE_DELETE(frustum);
	//SAFE_DELETE(projection);

	SAFE_DELETE(mouseRect);
}

void TestMultiPicking2::Update()
{
	plane->Update();

	frustum->Update();

	if (Mouse::Get()->Down(0)) {
		startPos = Mouse::Get()->GetPosition();
	}
	if (Mouse::Get()->Press(0)) {
		endPos = Mouse::Get()->GetPosition();

		drawRect = true;
	}
	if (Mouse::Get()->Up(0)) {
		endPos = Mouse::Get()->GetPosition();

		checkPicking = true;
	}
}


void TestMultiPicking2::Render()
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
				D3DXVECTOR3 start;
				values->MainCamera->Position(&start);

				float z = startPos.z;
				bool isPick = false;
				for (float x = startPos.x; x <= endPos.x; x += 5.0f)
				{
					for (float y = startPos.y; y <= endPos.y; y += 5.0f)
					{
						D3DXVECTOR3 dir = values->MainCamera->Direction(
							values->Viewport, values->Perspective, D3DXVECTOR3(x, y, z));
						
						if (sphere->IsMultiPickModel(start, dir))
						{
							isPick = true;
							break;
						}
					}
					if (isPick)
						break;
				}

				if (isPick)
					colors[i] = D3DXCOLOR(0, 0, 1, 1);
				else
					colors[i] = D3DXCOLOR(1, 1, 1, 1);

				// test
				//if (sphere->IsPickModel(values->MainCamera, values->Viewport, values->Perspective))
				//{
				//	colors[i] = D3DXCOLOR(0, 0, 1, 1);
				//}
				//else
				//	colors[i] = D3DXCOLOR(1, 1, 1, 1);

			}

			sphere->SetDiffuse(colors[i]);
			sphere->Update();
			sphere->Render();

			drawCount++;
		}
		i++;
	}

	// 사각형 그리기
	//if (drawRect) {
	//	D3DXVECTOR3 start = values->MainCamera->Direction(
	//		values->Viewport, values->Perspective, startPos);
	//	start *= 100;
	//	D3DXVECTOR3 end = values->MainCamera->Direction(
	//		values->Viewport, values->Perspective, endPos);
	//	end *= 100;


	//	D3DXVECTOR2 min = D3DXVECTOR2(start.x, start.y);
	//	D3DXVECTOR2 max = D3DXVECTOR2(end.x, end.y);

	//	float z = start.z;

	//	lines[0] = D3DXVECTOR3(min.x, min.y, z);
	//	lines[1] = D3DXVECTOR3(max.x, min.y, z);
	//	lines[2] = D3DXVECTOR3(max.x, max.y, z);
	//	lines[3] = D3DXVECTOR3(min.x, max.y, z);
	//	lines[4] = D3DXVECTOR3(min.x, min.y, z);

	//	mouseRect->SetLine(lines, 4, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	//	mouseRect->Render();
	//}

	if (checkPicking) {
		checkPicking = false;
		drawRect = false;
	}

	ImGui::Text("Draw %d / %d", drawCount, positions.size());
}

