#include "stdafx.h"
#include "MapTool.h"
#include "MapToolCommand\MapToolCommandCaretaker.h"
#include "MapToolCommand\MapToolCommands.h"
#include "MapToolCreateObject.h"
#include "MapToolBrush.h"
#include "MapToolData.h"
#include "./Objects/PickingObject.h"
#include "./Utilities/Gizmo.h"
#include "./Objects/GameTree.h"
#include "./Lights/PointLight.h"
#include "./Lights/SpotLight.h"
#include "./Lights/AreaLight.h"
#include "./Physics/RayTracingRT.h"

MapTool::MapTool(ExecuteValues * values)
	: values(values)
	, mouseType(MouseType::NONE)
	, mousePos(0, 0, 0), bMouseOnWindow(false), bPicked(false)
	, objectBrush(NULL), bTransformed(false)
	, lightBrush(0), lightLast(0)
{
	material = new Material(Shaders + L"020_TerrainSplatting.hlsl");

	mapData = new MapToolData();
	CreateBuffer(mapData);
	brush = new MapToolBrush(mapData);

	gridBuffer = new GridBuffer();


	rasterizer[0] = new RasterizerState();
	rasterizer[1] = new RasterizerState();
	rasterizer[1]->FillMode(D3D11_FILL_WIREFRAME);

	commands = new MapToolCommandCaretaker();
	pickingObj = new PickingObject(values);

	pointLight = new PointLight();
	spotLight = new SpotLight();
	areaLight = new AreaLight();

	treeTex = new Texture(Textures + L"Tree.png");

	rayTracing = new RayTracingRT(values);
}

MapTool::~MapTool()
{
	SAFE_DELETE(rayTracing);

	SAFE_DELETE(treeTex);

	SAFE_DELETE(pointLight);
	SAFE_DELETE(spotLight);
	SAFE_DELETE(areaLight);

	SAFE_DELETE(objectBrush);
	for (GameModel * object : objects)
		SAFE_DELETE(object);

	SAFE_DELETE(commands);

	SAFE_DELETE(rasterizer[0]);
	SAFE_DELETE(rasterizer[1]);

	SAFE_DELETE(material);

	SAFE_DELETE(brush);

	SAFE_DELETE(gridBuffer);
}

void MapTool::Update()
{
	D3DXVECTOR3 position;

	bool bCheck = (mousePos != Mouse::Get()->GetPosition()) ? true : false;
	if (bCheck)
	{
		bPicked = rayTracing->Picking(&position);
		brush->SetPosition(position);
		mousePos = Mouse::Get()->GetPosition();
	}

	bCheck &= Mouse::Get()->Press(0) == false;
	if (bCheck)
	{
		bMouseOnWindow = ImGui::IsMouseHoveringAnyWindow();
	}

	if (bMouseOnWindow == false && bPicked == true)
	{
		if (Mouse::Get()->Down(0))
		{
			if (mouseType == MouseType::NONE)
				pickingObj->Picking(objects);
			//else
				//MakeUndo();
			if (mouseType == MouseType::BRUSH)
				brush->Draw();
		}
		if (Mouse::Get()->Press(0))
			if (mouseType == MouseType::BRUSH)
				brush->Draw();
		if (Mouse::Get()->Up(0))
			if (mouseType != MouseType::NONE);
				//commands->PushUndoCommand();
	}

	if (Keyboard::Get()->Press(VK_CONTROL))
	{
		if (Keyboard::Get()->Down('Z'))
		{
			commands->UndoCommand();
			D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &mapData->vertices[0], sizeof(VertexTextureNormalTangentSplatting), mapData->vertexCount);
		}
		if (Keyboard::Get()->Down('Y'))
		{
			commands->RedoCommand();
			D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &mapData->vertices[0], sizeof(VertexTextureNormalTangentSplatting), mapData->vertexCount);
		}
	}

	for (GameModel * object : objects)
		object->Update();

	////pointLight Brush
	//if (lightBrush == lightLast)
	//{
	//	if (mouseType == DRAW_LIGHT)
	//	{
	//		switch (selectLight)
	//		{
	//		case LIGHT_POINT:
	//			if (pointLight->GetCount() > 0)
	//				pointLight->SetDesc(lightBrush)->Position = D3DXVECTOR3(brushBuffer->Data.Location.x, brushBuffer->Data.Location.y + pointLight->SetDesc(lightBrush)->Range * 0.5f, brushBuffer->Data.Location.z);
	//			break;
	//		case LIGHT_SPOT:
	//			if (spotLight->GetCount() > 0)
	//				spotLight->SetDesc(lightBrush)->Position = D3DXVECTOR3(brushBuffer->Data.Location.x, brushBuffer->Data.Location.y + spotLight->SetDesc(lightBrush)->Range * 0.5f, brushBuffer->Data.Location.z);
	//			break;
	//		case LIGHT_AREA:
	//			if (areaLight->GetCount() > 0)
	//				areaLight->SetDesc(lightBrush)->Position = D3DXVECTOR3(brushBuffer->Data.Location.x, brushBuffer->Data.Location.y, brushBuffer->Data.Location.z);
	//			break;
	//		}
	//		
	//	}
	//	else
	//	{
	//		switch (selectLight)
	//		{
	//		case LIGHT_POINT:
	//			pointLight->SetCount(lightLast);
	//			break;
	//		case LIGHT_SPOT:
	//			spotLight->SetCount(lightLast);
	//			break;
	//		case LIGHT_AREA:
	//			areaLight->SetCount(lightLast);
	//			break;
	//		}
	//	}
	//}
	//else
	//{
	//	switch (selectLight)
	//	{
	//	case LIGHT_POINT:
	//		pointLight->SetCount(lightLast);
	//		break;
	//	case LIGHT_SPOT:
	//		spotLight->SetCount(lightLast);
	//		break;
	//	case LIGHT_AREA:
	//		areaLight->SetCount(lightLast);
	//		break;
	//	}
	//}
	//
	//pointLight->Update();
	//spotLight->Update();
	//areaLight->Update();

	////ojbect Brush
	//if(objectBrush != NULL)
	//{
	//	D3DXVECTOR3 S;
	//	S.x = float(brushBuffer->Data.Range * 2);
	//	S.y = float(brushBuffer->Data.Range * 2);
	//	S.z = float(brushBuffer->Data.Range * 2);
	//
	//	objectBrush->Scale(S);
	//	objectBrush->Position(brushBuffer->Data.Location.x, brushBuffer->Data.Location.y + S.y * 0.5f, brushBuffer->Data.Location.z);
	//	objectBrush->Update();
	//}
}

void MapTool::PreRender()
{
	mapData->SetRender();
	material->PSSetBuffer();
	rayTracing->PreRender(vertexBuffer, indexBuffer, mapData->indexCount);
}

void MapTool::Render()
{	
	ImGuiRender();

	brush->SetRender();
	mapData->SetRender();

	gridBuffer->SetPSBuffer(10);

	UINT stride = sizeof(VertexTextureNormalTangentSplatting);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	material->PSSetBuffer();

	if (bWireframe == true)
		rasterizer[1]->RSSetState();
	D3D::GetDC()->DrawIndexed(mapData->indexCount, 0, 0);
	rasterizer[0]->RSSetState();

	//for (GameModel * object : objects)
	//	object->Render();
	//
	//if(objectBrush != NULL && mouseType == MouseType::BRUSH)
	//	objectBrush->Render();

	//if (mouseType == DRAW_LIGHT)
	//{
	//	switch (selectLight)
	//	{
	//	case LIGHT_POINT:
	//		pointLight->GizmoRender((UINT)lightBrush);
	//		break;
	//	case LIGHT_SPOT:
	//		spotLight->Render((UINT)lightBrush);
	//		break;
	//	case LIGHT_AREA:
	//		areaLight->Render((UINT)lightBrush);
	//		break;
	//	}
	//}
}

void MapTool::PostRender()
{
}

void MapTool::ResizeScreen()
{
}

float MapTool::Y(D3DXVECTOR3& position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x >= mapData->width) return 0.0f;
	if (z < 0 || z >= mapData->height) return 0.0f;

	UINT index[4];
	index[0] = (mapData->width + 1) * z + x;
	index[1] = (mapData->width + 1) * (z + 1) + x;
	index[2] = (mapData->width + 1) * z + (x + 1);
	index[3] = (mapData->width + 1) * (z + 1) + (x + 1);

	D3DXVECTOR3 v[4];
	for (int i = 0; i < 4; i++)
		v[i] = mapData->vertices[index[i]].Position;

	float ddx = (position.x - v[0].x) / 1.0f;
	float ddz = (position.z - v[0].z) / 1.0f;

	D3DXVECTOR3 temp;

	if (ddx + ddz <= 1)
	{
		temp = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	}
	else
	{
		ddx = 1 - ddx;
		ddz = 1 - ddz;

		temp = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}

	return temp.y;
}

bool MapTool::Y(OUT D3DXVECTOR3 * out, D3DXVECTOR3 & position)
{
	D3DXVECTOR3 start = D3DXVECTOR3(position.x, 1000, position.z);
	D3DXVECTOR3 direction = D3DXVECTOR3(0, -1, 0);

	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x >= mapData->width) return 0.0f;
	if (z < 0 || z >= mapData->height) return 0.0f;

	UINT index[4];
	index[0] = (mapData->width + 1) * z + x;
	index[1] = (mapData->width + 1) * (z + 1) + x;
	index[2] = (mapData->width + 1) * z + (x + 1);
	index[3] = (mapData->width + 1) * (z + 1) + (x + 1);

	D3DXVECTOR3 p[4];
	for (int i = 0; i < 4; i++)
		p[i] = mapData->vertices[index[i]].Position;

	float u, v, distance;

	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance))
	{
		*out = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;
		return true;
	}
	else if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance))
	{
		*out = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;
		return true;
	}
	return false;
}

bool MapTool::Picked(OUT D3DXVECTOR3 * out)
{
	D3DXVECTOR3 start;
	values->MainCamera->Position(&start);
	
	D3DXVECTOR3 direction = values->MainCamera->Direction(values->Viewport, values->Perspective);
	
	for (UINT z = 0; z < mapData->height; z++)
	{
		for (UINT x = 0; x < mapData->width; x++)
		{
			UINT index[4];
			index[0] = (mapData->width + 1) * z + x;
			index[1] = (mapData->width + 1) * (z + 1) + x;
			index[2] = (mapData->width + 1) * z + (x + 1);
			index[3] = (mapData->width + 1) * (z + 1) + (x + 1);
	
			D3DXVECTOR3 p[4];
			for (int i = 0; i < 4; i++)
				p[i] = mapData->vertices[index[i]].Position;
	
			float u, v, distance;
	
			if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance))
			{
				*out = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;
				return true;
			}
			else if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance))
			{
				*out = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;
				return true;
			}
		}//for(x)
	}//for(z)
	return false;
}

bool MapTool::Pick2D(OUT D3DXVECTOR3 * out)
{
	return false;
}

void MapTool::CreateBuffer(MapToolData* mapData)
{
	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTextureNormalTangentSplatting) * mapData->vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = mapData->vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//CreateIndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * mapData->indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = mapData->indices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void MapTool::ImGuiRender()
{
	ImGui::Begin("MapTool");
	{
		D3DXVECTOR3 mPos = brush->GetPosition();
		ImGui::Text("brush %f, %f", mPos.x, mPos.z);
		ImGui::Text("mouse %f, %f", Mouse::Get()->GetPosition().x, Mouse::Get()->GetPosition().y);

		ImGui::Separator();
		ImGui::Checkbox("Terrain Wireframe", &bWireframe);
		{
			if (ImGui::Button("Undo"))
			{
				commands->UndoCommand();
				D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &mapData->vertices[0], sizeof(VertexTextureNormalTangentSplatting), mapData->vertexCount);
			}
			ImGui::SameLine(80);
		}
		{
			if (ImGui::Button("Redo"))
			{
				commands->RedoCommand();
				D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &mapData->vertices[0], sizeof(VertexTextureNormalTangentSplatting), mapData->vertexCount);
			}
		}
		ImGui::Separator();
		ImGui::NewLine();
		ImGui::SameLine(20);
		if (ImGui::Button("Cursor", ImVec2(80, 20)))
		{
			mouseType = MouseType::NONE;
		}
		ImGui::SameLine(110);
		if (ImGui::Button("Brush", ImVec2(80, 20)))
		{
			mouseType = MouseType::BRUSH;
		}
		ImGui::SameLine(200);
		//if (ImGui::Button("Light", ImVec2(80, 20)))
		//{
		//	if (selectTap != 2)
		//		mouseType = DRAW_LIGHT;
		//	selectTap = 2;
		//}
		ImGui::Separator();

		
		//else if (selectTap == 1)
		//{
		//	ImGui::Text("Object");
		//	ImGui::Separator();
		//	ImGui::Text("mouseType");
		//	ImGui::RadioButton("Select", (int*)&mouseType, DRAW_SELECT); ImGui::SameLine(80);
		//	ImGui::RadioButton("Create", (int*)&mouseType, DRAW_OBJECT); ImGui::SameLine(160);
		//	if (ImGui::RadioButton("Light", (int*)&mouseType, DRAW_LIGHT))
		//	{
		//		lightLast = pointLight->GetCount();
		//		lightBrush = lightLast;
		//		PointLight::Desc desc;
		//		desc.Position = D3DXVECTOR3(0, 0, 0);
		//		desc.Range = 1;
		//		desc.Color = D3DXVECTOR3(1, 1, 1);
		//		desc.Intensity = 0.1f;
		//		pointLight->Add(desc);
		//	}
		//	if (mouseType == DRAW_SELECT)
		//	{
		//		if (pickingObj->GetPickedObj() != NULL)
		//			pickingObj->GetPickedObj()->DebugRender();
		//	}
		//	if (mouseType == DRAW_OBJECT)
		//	{
		//		ImGui::Separator();
		//		if (ImGui::Button("Cube", ImVec2(50, 50)))
		//			ChangeObjectBrush(ObjType_Cube);
		//		ImGui::SameLine(60);
		//		if (ImGui::Button("Sphere", ImVec2(50, 50)))
		//			ChangeObjectBrush(ObjType_Sphere);
		//		ImGui::SameLine(120);
		//		if (ImGui::ImageButton(treeTex->GetView(), ImVec2(44, 44)))
		//			ChangeObjectBrush(ObjType_Tree);
		//	}
		//	ImGui::Separator();
		//	ImGui::SliderInt("Size", &brushBuffer->Data.Range, 1, 10);
		//}
		//if (selectTap == 2)
		//{
		//	ImGui::NewLine();
		//	ImGui::SameLine(20);
		//	if (ImGui::RadioButton("PointLight", (int*)&selectLight, LIGHT_POINT))
		//	{
		//		lightLast = pointLight->GetCount();
		//		lightBrush = lightLast;
		//		PointLight::Desc desc;
		//		desc.Position = D3DXVECTOR3(0, 0, 0);
		//		desc.Range = 1;
		//		desc.Color = D3DXVECTOR3(1, 1, 1);
		//		desc.Intensity = 0.1f;
		//		pointLight->Add(desc);
		//	}
		//	if (ImGui::RadioButton("SpotLight", (int*)&selectLight, LIGHT_SPOT))
		//	{
		//		lightLast = spotLight->GetCount();
		//		lightBrush = lightLast;
		//		SpotLight::Desc desc;
		//		desc.Position = D3DXVECTOR3(0, 0, 0);
		//		desc.Range = 1;
		//		desc.Color = D3DXVECTOR3(1, 1, 1);
		//		desc.Angle = 0.1f;
		//		desc.Direction = D3DXVECTOR3(0, -1, 0);
		//		spotLight->Add(desc);
		//	}
		//	if (ImGui::RadioButton("AreaLight", (int*)&selectLight, LIGHT_AREA))
		//	{
		//		lightLast = areaLight->GetCount();
		//		lightBrush = lightLast;
		//		AreaLight::Desc desc;
		//		desc.Position = D3DXVECTOR3(0, 0, 0);
		//		desc.Width = 1.0f;
		//		desc.Color = D3DXVECTOR3(1, 1, 1);
		//		desc.Height = 1.0f;
		//		desc.Direction = D3DXVECTOR3(0, -1, 0);
		//		desc.Intensity = 1.0f;
		//		areaLight->Add(desc);
		//	}
		//	ImGui::Separator();
		//	switch (selectLight)
		//	{
		//	case LIGHT_POINT:
		//	{
		//		ImGui::SliderInt("LightNum", &lightBrush, 0, (int)pointLight->GetCount());
		//		{
		//
		//			if (lightBrush == pointLight->GetCount())
		//			{
		//				lightLast = pointLight->GetCount();
		//				lightBrush = lightLast;
		//				PointLight::Desc desc;
		//				desc.Position = D3DXVECTOR3(0, 0, 0);
		//				desc.Range = 1;
		//				desc.Color = D3DXVECTOR3(1, 1, 1);
		//				desc.Intensity = 0.1f;
		//				pointLight->Add(desc);
		//			}
		//		}
		//		ImGui::DragFloat3("Position", (float*)&pointLight->SetDesc(lightBrush)->Position, 0.1f, 0.0f, (float)width, "%.2f");
		//		ImGui::DragFloat("Range", &pointLight->SetDesc(lightBrush)->Range, 0.1f, 0.0f, 10.0f, "%.2f");
		//		ImGui::DragFloat("Intensity", &pointLight->SetDesc(lightBrush)->Intensity, 0.1f, 0.1f, 20.0f, "%.2f");
		//		ImGui::DragFloat3("Color", (float*)&pointLight->SetDesc(lightBrush)->Color, 0.01f, 0.0f, 1.0f, "%.2f");
		//		break;
		//	}
		//	case LIGHT_SPOT:
		//	{
		//		ImGui::SliderInt("LightNum", &lightBrush, 0, (int)spotLight->GetCount());
		//		{
		//
		//			if (lightBrush == spotLight->GetCount())
		//			{
		//				lightLast = spotLight->GetCount();
		//				lightBrush = lightLast;
		//				SpotLight::Desc desc;
		//				desc.Position = D3DXVECTOR3(0, 0, 0);
		//				desc.Range = 1;
		//				desc.Color = D3DXVECTOR3(1, 1, 1);
		//				desc.Angle = 0.1f;
		//				desc.Direction = D3DXVECTOR3(0, -1, 0);
		//				spotLight->Add(desc);
		//			}
		//		}
		//		float angle = Math::ToDegree(spotLight->SetDesc(lightBrush)->Angle);
		//		ImGui::DragFloat3("Position", (float*)&spotLight->SetDesc(lightBrush)->Position, 0.1f, 0.0f, (float)width, "%.2f");
		//		ImGui::DragFloat("Range", &spotLight->SetDesc(lightBrush)->Range, 0.1f, 0.0f, 10.0f, "%.2f");
		//		if (ImGui::DragFloat("Angle", &angle, 0.1f, 0.0f, 180.0f, "%.2f"))
		//			spotLight->SetDesc(lightBrush)->Angle = Math::ToRadian(angle);
		//		ImGui::DragFloat3("Direction", (float*)&spotLight->SetDesc(lightBrush)->Direction, 0.01f, -1.0f, 1.0f, "%.2f");
		//		ImGui::DragFloat3("Color", (float*)&spotLight->SetDesc(lightBrush)->Color, 0.01f, 0.0f, 1.0f, "%.2f");
		//		break;
		//	}
		//	case LIGHT_AREA:
		//	{
		//		ImGui::SliderInt("LightNum", &lightBrush, 0, (int)areaLight->GetCount());
		//		{
		//
		//			if (lightBrush == areaLight->GetCount())
		//			{
		//				lightLast = areaLight->GetCount();
		//				lightBrush = lightLast;
		//				AreaLight::Desc desc;
		//				desc.Position = D3DXVECTOR3(0, 0, 0);
		//				desc.Width = 5.0f;
		//				desc.Color = D3DXVECTOR3(1, 1, 1);
		//				desc.Height = 5.0f;
		//				desc.Direction = D3DXVECTOR3(0, -1, 0);
		//				desc.Intensity = 10.0f;
		//				areaLight->Add(desc);
		//			}
		//		}
		//		ImGui::DragFloat3("Position", (float*)&areaLight->SetDesc(lightBrush)->Position, 0.1f, 0.0f, (float)width, "%.2f");
		//		ImGui::DragFloat("Width", &areaLight->SetDesc(lightBrush)->Width, 0.1f, 0.0f, 10.0f, "%.2f");
		//		ImGui::DragFloat("Height", &areaLight->SetDesc(lightBrush)->Height, 0.1f, 0.0f, 10.0f, "%.2f");
		//		ImGui::DragFloat("Intensity", &areaLight->SetDesc(lightBrush)->Intensity, 0.1f, 0.1f, 20.0f, "%.2f");
		//		ImGui::DragFloat3("Direction", (float*)&areaLight->SetDesc(lightBrush)->Direction, 0.01f, -1.0f, 1.0f, "%.2f");
		//		ImGui::DragFloat3("Color", (float*)&areaLight->SetDesc(lightBrush)->Color, 0.01f, 0.0f, 1.0f, "%.2f");
		//		break;
		//	}
		//	break;
		//	}
		//}
		ImGui::Separator();
		ImGui::Text("LineType");
		ImGui::SliderInt("LineType", (int*)&gridBuffer->Data.Type, 0, 2);
		ImGui::SliderFloat3("LineColor", (float*)&gridBuffer->Data.Color, 0, 1);
		ImGui::SliderInt("Line Spacing", &gridBuffer->Data.Spacing, 1, 10);
		ImGui::SliderFloat("LineThickness", (float*)&gridBuffer->Data.Thickness, 0.01f, 0.5f);
		ImGui::Separator();
		ImGui::Spacing();
	}
	ImGui::End();
}

//void MapTool::ChangeObjectBrush(ObjType object)
//{
//	selectObject = object;
//	SAFE_DELETE(objectBrush);
//	objectBrush = CreateObjectType[object](values, D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1));
//	objectBrush->Update();
//}
//
//void MapTool::CreateObject(D3DXVECTOR3 & location)
//{
//	D3DXVECTOR3 S;
//	S.x = float(brushBuffer->Data.Range * 2);
//	S.y = float(brushBuffer->Data.Range * 2);
//	S.z = float(brushBuffer->Data.Range * 2);
//	objects.push_back(CreateObjectType[selectObject](values, location, S));
//}
//
//void MapTool::CreateLight(D3DXVECTOR3 & location)
//{
//	switch (selectLight)
//	{
//	case LIGHT_POINT:
//	{
//		lightLast = pointLight->GetCount();
//		lightBrush = lightLast;
//		PointLight::Desc desc;
//		desc.Position = location;
//		desc.Range = 1;
//		desc.Color = D3DXVECTOR3(1, 1, 1);
//		desc.Intensity = 0.1f;
//		pointLight->Add(desc);
//		break;
//	}
//	case LIGHT_SPOT:
//	{
//		lightLast = spotLight->GetCount();
//		lightBrush = lightLast;
//		SpotLight::Desc desc;
//		desc.Position = location;
//		desc.Range = 1;
//		desc.Color = D3DXVECTOR3(1, 1, 1);
//		desc.Angle = 0.1f;
//		desc.Direction = D3DXVECTOR3(location.x, location.y - 1.0f, location.z);
//		spotLight->Add(desc);
//		break;
//	}
//	case LIGHT_AREA:
//	{
//		lightLast = areaLight->GetCount();
//		lightBrush = lightLast;
//		AreaLight::Desc desc;
//		desc.Position = location;
//		desc.Width = 5.0f;
//		desc.Color = D3DXVECTOR3(1, 1, 1);
//		desc.Height = 5.0f;
//		desc.Direction = D3DXVECTOR3(location.x, location.y - 1.0f, location.z);
//		desc.Intensity = 10.0f;
//		areaLight->Add(desc);
//		break;
//	}
//		break;
//	case LIGHT_END:
//		break;
//	default:
//		break;
//	}
//}
//
//
//
//void MapTool::MakeUndo()
//{
//	switch (mouseType)
//	{
//	case DRAW_UP:
//		commands->MakeUndoCommand(new MapToolDrawLand(vertices, vertexCount));
//		break;
//	case DRAW_DOWN:
//		commands->MakeUndoCommand(new MapToolDrawLand(vertices, vertexCount));
//		break;
//	case DRAW_FLAT:
//		commands->MakeUndoCommand(new MapToolDrawLand(vertices, vertexCount));
//		break;
//	case DRAW_TEXTURE:
//		commands->MakeUndoCommand(new MapToolDrawLand(vertices, vertexCount));
//		break;
//	case DRAW_OBJECT:
//		commands->MakeUndoCommand(new MapToolCreateObejct(&objects));
//		break;
//	case DRAW_SELECT:
//		commands->MakeUndoCommand(new MapToolMoveObejct(pickingObj->GetPickedObj()));
//		break;
//	case DRAW_END:
//		break;
//	default:
//		break;
//	}
//}
