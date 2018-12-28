#include "stdafx.h"
#include "MapToolBrush.h"
#include "MapToolData.h"
#include "MapTool.h"

MapToolBrush::MapToolBrush(MapToolData* mapData)
	: mapData(mapData)
	, flatHeight(10.0f), drawConcentration(10.0f), selectTexture(0)
	, selectTap(0)
{
	textures[0] = new Texture(Textures + L"Dirt.png");
	mapData->texFileName[0] = Textures + L"Dirt.png";
	texName[0] = "Dirt";
	
	for (UINT i = 1; i < 5; i++)
		textures[i] = NULL;

	brushBuffer = new BrushBuffer();
}

MapToolBrush::~MapToolBrush()
{
	for (UINT i = 0; i < 4; i++)
		SAFE_DELETE(textures[i]);

	SAFE_DELETE(brushBuffer);
}

void MapToolBrush::Draw()
{
	D3DXVECTOR3 location;
	switch (drawType)
	{
	case DrawType::UP:
		TerrainUp(location);
		break;
	case DrawType::DOWN:
		TerrainDown(location);
		break;
	case DrawType::FLAT:
		TerrainFlat(location);
		break;
	case DrawType::TEXTURE:
		TerrainTexture(location);
		break;
	case DrawType::OBJECT:
		//CreateObject(location);
		break;
	//case DrawType::LIGHT:
	//	CreateLight(location);
	//	break;
	default:
		break;
	}

	//D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, &vertices[0], sizeof(VertexTextureNormalTangentSplatting), vertexCount);
}

void MapToolBrush::SetRender()
{
	brushBuffer->SetVSBuffer(10);

	for (UINT i = 0; i < 5; i++)
	{
		if (textures[i] == NULL) continue;
		textures[i]->SetShaderResource(10 + i);
		textures[i]->SetShaderSampler(10 + i);
	}
}

void MapToolBrush::ImGuiRender()
{
	if (selectTap == 0)
	{
		ImGui::Text("DrawType");
		ImGui::RadioButton("Up", (int*)&drawType, (int)DrawType::UP); ImGui::SameLine(80);
		ImGui::RadioButton("Down", (int*)&drawType, (int)DrawType::DOWN); ImGui::SameLine(160);
		ImGui::RadioButton("Flat", (int*)&drawType, (int)DrawType::FLAT); ImGui::SameLine(240);
		ImGui::RadioButton("Texture", (int*)&drawType, (int)DrawType::TEXTURE);
		ImGui::Separator();
		ImGui::Text("BrushType");
		ImGui::RadioButton("Default", &brushBuffer->Data.BrushType, 1); ImGui::SameLine(180);
		ImGui::RadioButton("Spread", &brushBuffer->Data.BrushType, 2);
		ImGui::SliderInt("Range", &brushBuffer->Data.Range, 1, 10);
		ImGui::SliderFloat("Concentration", &drawConcentration, 1.0f, 20.f);
		if (drawType == DrawType::FLAT)
			ImGui::SliderFloat("Height", &flatHeight, 0.0f, 20.f);
		if (drawType == DrawType::TEXTURE)
		{
			if (mapData->textureCount < 5)
			{
				if (ImGui::Button("AddTexture"))
				{
					selectTexture = mapData->textureCount;
					func = bind(&MapToolBrush::LoadDrawTexture, this, placeholders::_1);
					Path::OpenFileDialog(L"", L"PNG Files(*.png)\0*.png\0", Textures, func);
				}
			}
			for (UINT i = 0; i < mapData->textureCount; i++)
			{
				ImGui::RadioButton(texName[i].c_str(), (int*)&selectTexture, i);
				if (i < mapData->textureCount - 1)
					ImGui::SameLine(67 + 59 * (float)i);
			}
			for (UINT i = 0; i < mapData->textureCount; i++)
			{
				if (ImGui::ImageButton(textures[i]->GetView(), ImVec2(50, 50)))
				{
					//텍스쳐 로드
					selectTexture = i;
					func = bind(&MapToolBrush::LoadDrawTexture, this, placeholders::_1);
					Path::OpenFileDialog(L"", L"PNG Files(*.png)\0*.png\0", Textures, func);
				}
				if (i < mapData->textureCount - 1)
					ImGui::SameLine(67 + 59 * (float)i);
			}
		}
		ImGui::Separator();
		if (ImGui::Button("SaveHeightMap"))
		{
			func = bind(&MapToolBrush::SaveHeightMap, this, placeholders::_1);
			Path::SaveFileDialog(L"", L"PNG Files(*.png)\0*.png\0", Contents + L"HeightMaps/", func);
		}
		ImGui::SameLine(120);
		if (ImGui::Button("LoadHeightMap"))
		{
			func = bind(&MapToolBrush::LoadHeightMap, this, placeholders::_1);
			Path::OpenFileDialog(L"", L"PNG Files(*.png)\0*.png\0", Contents + L"HeightMaps/", func);
		}
		if (ImGui::Button("SaveAlphaMap"))
		{
			func = bind(&MapToolBrush::SaveAlphaMap, this, placeholders::_1);
			Path::SaveFileDialog(L"", L"PNG Files(*.png)\0*.png\0", Contents + L"HeightMaps/", func);
		}
		ImGui::SameLine(120);
		if (ImGui::Button("LoadAlphaMap"))
		{
			func = bind(&MapToolBrush::LoadAlphaMap, this, placeholders::_1);
			Path::OpenFileDialog(L"", L"PNG Files(*.png)\0*.png\0", Contents + L"HeightMaps/", func);
		}
	}
}

void MapToolBrush::TerrainUp(D3DXVECTOR3 & location)
{
	UINT size = (UINT)brushBuffer->Data.Range;
	UINT sizePow = size * size;

	D3D11_BOX box;
	box.left = (UINT)location.x - size;
	box.top = (UINT)location.z + size;
	box.right = (UINT)location.x + size;
	box.bottom = (UINT)location.z - size;

	if (box.left < 0)		box.left = 0;
	if (box.top >= mapData->height)	box.top = mapData->height;
	if (box.right >= mapData->width)	box.right = mapData->width;
	if (box.bottom < 0)		box.bottom = 0;

	switch (brushBuffer->Data.BrushType)
	{
	case 1:
		for (UINT z = box.bottom; z <= box.top; z++)
		{
			for (UINT x = box.left; x <= box.right; x++)
			{
				float distPow = (location.x - x) * (location.x - x) + (location.z - z) * (location.z - z);

				if (distPow > sizePow) continue;

				UINT index = (mapData->width + 1) * z + x;

				mapData->vertices[index].Position.y += drawConcentration * Time::Delta();
			}
		}
		break;
	case 2:
		for (UINT z = box.bottom; z <= box.top; z++)
		{
			for (UINT x = box.left; x <= box.right; x++)
			{
				float distPow = (location.x - x) * (location.x - x) + (location.z - z) * (location.z - z);

				if (distPow > sizePow) continue;

				UINT index = (mapData->width + 1) * z + x;

				mapData->vertices[index].Position.y += drawConcentration * Time::Delta() * (0.5f * cosf(Math::PI * sqrt(distPow) / (float)size) + 0.5f);
			}
		}
		break;
	}
}

void MapToolBrush::TerrainDown(D3DXVECTOR3 & location)
{
	UINT size = (UINT)brushBuffer->Data.Range;
	UINT sizePow = size * size;

	D3D11_BOX box;
	box.left = (UINT)location.x - size;
	box.top = (UINT)location.z + size;
	box.right = (UINT)location.x + size;
	box.bottom = (UINT)location.z - size;

	if (box.left < 0)		box.left = 0;
	if (box.top >= mapData->height)	box.top = mapData->height;
	if (box.right >= mapData->width)	box.right = mapData->width;
	if (box.bottom < 0)		box.bottom = 0;

	switch (brushBuffer->Data.BrushType)
	{
	case 1:
		for (UINT z = box.bottom; z <= box.top; z++)
		{
			for (UINT x = box.left; x <= box.right; x++)
			{
				float distPow = (location.x - x) * (location.x - x) + (location.z - z) * (location.z - z);

				if (distPow > sizePow)
					continue;

				UINT index = (mapData->width + 1) * z + x;

				mapData->vertices[index].Position.y -= drawConcentration * Time::Delta();

				if (mapData->vertices[index].Position.y < 0)
					mapData->vertices[index].Position.y = 0;
			}
		}
		break;
	case 2:
		for (UINT z = box.bottom; z <= box.top; z++)
		{
			for (UINT x = box.left; x <= box.right; x++)
			{
				float distPow = (location.x - x) * (location.x - x) + (location.z - z) * (location.z - z);

				if (distPow > sizePow)
					continue;

				UINT index = (mapData->width + 1) * z + x;

				mapData->vertices[index].Position.y -= drawConcentration * Time::Delta() * (0.5f * cosf(Math::PI * sqrt(distPow) / (float)size) + 0.5f);

				if (mapData->vertices[index].Position.y < 0)
					mapData->vertices[index].Position.y = 0;
			}
		}
		break;
	}
}

void MapToolBrush::TerrainFlat(D3DXVECTOR3 & location)
{
	UINT size = (UINT)brushBuffer->Data.Range;
	UINT sizePow = size * size;

	D3D11_BOX box;
	box.left = (UINT)location.x - size;
	box.top = (UINT)location.z + size;
	box.right = (UINT)location.x + size;
	box.bottom = (UINT)location.z - size;

	if (box.left < 0)		box.left = 0;
	if (box.top >= mapData->height)	box.top = mapData->height;
	if (box.right >= mapData->width)	box.right = mapData->width;
	if (box.bottom < 0)		box.bottom = 0;

	switch (brushBuffer->Data.BrushType)
	{
	case 1:
		for (UINT z = box.bottom; z <= box.top; z++)
		{
			for (UINT x = box.left; x <= box.right; x++)
			{
				float distPow = (location.x - x) * (location.x - x) + (location.z - z) * (location.z - z);

				if (distPow > sizePow)
					continue;

				UINT index = (mapData->width + 1) * z + x;
				float drawCon = drawConcentration * Time::Delta();

				if (mapData->vertices[index].Position.y > flatHeight)
					mapData->vertices[index].Position.y -= drawCon;
				else if (mapData->vertices[index].Position.y < flatHeight)
					mapData->vertices[index].Position.y += drawCon;

				if (abs(mapData->vertices[index].Position.y - flatHeight) < drawCon)
					mapData->vertices[index].Position.y = flatHeight;
			}
		}
		break;
	case 2:
		for (UINT z = box.bottom; z <= box.top; z++)
		{
			for (UINT x = box.left; x <= box.right; x++)
			{
				float distPow = (location.x - x) * (location.x - x) + (location.z - z) * (location.z - z);

				if (distPow > sizePow)
					continue;

				UINT index = (mapData->width + 1) * z + x;
				float drawCon = drawConcentration * Time::Delta() * (0.5f * cosf(Math::PI * sqrt(distPow) / (float)size) + 0.5f);;

				if (mapData->vertices[index].Position.y > flatHeight)
					mapData->vertices[index].Position.y -= drawCon;
				else if (mapData->vertices[index].Position.y < flatHeight)
					mapData->vertices[index].Position.y += drawCon;

				if (abs(mapData->vertices[index].Position.y - flatHeight) < drawCon)
					mapData->vertices[index].Position.y = flatHeight;
			}
		}
		break;
	}
}

void MapToolBrush::TerrainTexture(D3DXVECTOR3 & location)
{
	UINT size = (UINT)brushBuffer->Data.Range;
	UINT sizePow = size * size;

	D3D11_BOX box;
	box.left = (UINT)location.x - size;
	box.top = (UINT)location.z + size;
	box.right = (UINT)location.x + size;
	box.bottom = (UINT)location.z - size;

	if (box.left < 0)		box.left = 0;
	if (box.top >= mapData->height)	box.top = mapData->height;
	if (box.right >= mapData->width)	box.right = mapData->width;
	if (box.bottom < 0)		box.bottom = 0;

	switch (brushBuffer->Data.BrushType)
	{
	case 1:
		for (UINT z = box.bottom; z <= box.top; z++)
		{
			for (UINT x = box.left; x <= box.right; x++)
			{
				float distPow = (location.x - x) * (location.x - x) + (location.z - z) * (location.z - z);

				if (distPow > sizePow)
					continue;

				UINT index = (mapData->width + 1) * z + x;
				float drawCon = drawConcentration * Time::Delta();

				switch (selectTexture)
				{
				case 0:
					mapData->vertices[index].Splatting.x -= drawCon;
					mapData->vertices[index].Splatting.y -= drawCon;
					mapData->vertices[index].Splatting.z -= drawCon;
					mapData->vertices[index].Splatting.w -= drawCon;
					if (mapData->vertices[index].Splatting.x < 1) mapData->vertices[index].Splatting.x = 0;
					if (mapData->vertices[index].Splatting.y < 1) mapData->vertices[index].Splatting.y = 0;
					if (mapData->vertices[index].Splatting.z < 1) mapData->vertices[index].Splatting.z = 0;
					if (mapData->vertices[index].Splatting.w < 1) mapData->vertices[index].Splatting.w = 0;
					break;
				case 1:
					mapData->vertices[index].Splatting.x += drawCon;
					if (mapData->vertices[index].Splatting.x > 1) mapData->vertices[index].Splatting.x = 1;
					break;
				case 2:
					mapData->vertices[index].Splatting.y += drawCon;
					if (mapData->vertices[index].Splatting.y > 1)mapData->vertices[index].Splatting.y = 1;
					break;
				case 3:
					mapData->vertices[index].Splatting.z += drawCon;
					if (mapData->vertices[index].Splatting.z > 1) mapData->vertices[index].Splatting.z = 1;
					break;
				case 4:
					mapData->vertices[index].Splatting.w += drawCon;
					if (mapData->vertices[index].Splatting.w > 1) mapData->vertices[index].Splatting.w = 1;
					break;
				}
			}
		}
		break;
	case 2:
		for (UINT z = box.bottom; z <= box.top; z++)
		{
			for (UINT x = box.left; x <= box.right; x++)
			{
				float distPow = (location.x - x) * (location.x - x) + (location.z - z) * (location.z - z);

				if (distPow > sizePow)
					continue;

				UINT index = (mapData->width + 1) * z + x;
				float drawCon = drawConcentration * Time::Delta() * (0.5f * cosf(Math::PI * sqrt(distPow) / (float)size) + 0.5f);;

				switch (selectTexture)
				{
				case 0:
					mapData->vertices[index].Splatting.x -= drawCon;
					mapData->vertices[index].Splatting.y -= drawCon;
					mapData->vertices[index].Splatting.z -= drawCon;
					mapData->vertices[index].Splatting.w -= drawCon;
					if (mapData->vertices[index].Splatting.x < 0) mapData->vertices[index].Splatting.x = 0;
					if (mapData->vertices[index].Splatting.y < 0) mapData->vertices[index].Splatting.y = 0;
					if (mapData->vertices[index].Splatting.z < 0) mapData->vertices[index].Splatting.z = 0;
					if (mapData->vertices[index].Splatting.w < 0) mapData->vertices[index].Splatting.w = 0;
					break;
				case 1:
					mapData->vertices[index].Splatting.x += drawCon;
					if (mapData->vertices[index].Splatting.x > 1) mapData->vertices[index].Splatting.x = 1;
					break;
				case 2:
					mapData->vertices[index].Splatting.y += drawCon;
					if (mapData->vertices[index].Splatting.y > 1) mapData->vertices[index].Splatting.y = 1;
					break;
				case 3:
					mapData->vertices[index].Splatting.z += drawCon;
					if (mapData->vertices[index].Splatting.z > 1) mapData->vertices[index].Splatting.z = 1;
					break;
				case 4:
					mapData->vertices[index].Splatting.w += drawCon;
					if (mapData->vertices[index].Splatting.w > 1) mapData->vertices[index].Splatting.w = 1;
					break;
				}
			}
		}
		break;
	}
	//D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertices, sizeof(VertexTextureNormalTangentSplatting) * vertexCount, 0);
}

void MapToolBrush::LoadDrawTexture(wstring fileName)
{
	if (textures[selectTexture] == NULL)
		mapData->textureCount++;
	else
		SAFE_DELETE(textures[selectTexture]);

	texName[selectTexture] = String::ToString(Path::GetFileNameWithoutExtension(fileName));
	textures[selectTexture] = new Texture(fileName);
}

void MapToolBrush::SaveAlphaMap(wstring fileName)
{
	Texture::WritePixels(fileName, mapData->vertices, mapData->height, mapData->width);
}

void MapToolBrush::LoadAlphaMap(wstring fileName)
{
	Texture* alphaTexture = new Texture(fileName);
	vector<D3DXCOLOR> color;
	alphaTexture->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &color);

	for (UINT z = 0; z <= mapData->height; z++)
	{
		for (UINT x = 0; x <= mapData->width; x++)
		{
			UINT count = (mapData->width + 1) * z + x;
			mapData->vertices[count].Splatting = (D3DXVECTOR4)color[count];
		}
	}

	//D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertices, sizeof(VertexTextureNormalTangentSplatting) * vertexCount, 0);
	SAFE_DELETE(alphaTexture);
}

void MapToolBrush::SaveHeightMap(wstring fileName)
{
	//heightTexture->SaveFile(fileName);
}

void MapToolBrush::LoadHeightMap(wstring fileName)
{
	mapData->LoadHeightMap(fileName);
}