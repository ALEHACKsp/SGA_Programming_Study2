#include "stdafx.h"
#include "MapToolData.h"

MapToolData::MapToolData()
{
	LoadHeightMap(Contents + L"HeightMaps/HeightMap256.png");
	worldBuffer = new WorldBuffer();
}

MapToolData::~MapToolData()
{
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(vertices);
	SAFE_DELETE(indices);
}

void MapToolData::SetRender()
{
	worldBuffer->SetVSBuffer(1);
}

void MapToolData::SaveMapData()
{
}

void MapToolData::LoadMapData()
{
}

void MapToolData::LoadHeightMap(wstring fileName)
{
	CreateVertexData(fileName);
	CreateNormalData();
	CreateTangentData();
}

void MapToolData::CreateVertexData(wstring fileName)
{
	vector<D3DXCOLOR> heights;

	Texture* heightTexture = new Texture(fileName);
	heightTexture->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	width = heightTexture->GetWidth() - 1;
	height = heightTexture->GetHeight() - 1;

	//Create VertexData
	{
		vertexCount = (width + 1) * (height + 1);
		vertices = new VertexTextureNormalTangentSplatting[vertexCount];

		for (UINT z = 0; z <= height; z++)
		{
			for (UINT x = 0; x <= width; x++)
			{
				UINT index = (width + 1) * z + x;

				vertices[index].Position.x = (float)x;
				vertices[index].Position.y = (float)(heights[index].r * 255.0f) / 10.0f;
				//vertices[index].Position.y = 0.0f;
				vertices[index].Position.z = (float)z;

				vertices[index].Uv.x = (float)x / width;
				vertices[index].Uv.y = (float)z / height;
			}
		}//for(z)
	}

	SAFE_DELETE(heightTexture);

	//CreateIndexData
	{
		indexCount = width * height * 6;
		indices = new UINT[indexCount];

		UINT index = 0;
		for (UINT z = 0; z < height; z++)
		{
			for (UINT x = 0; x < width; x++)
			{
				indices[index + 0] = (width + 1) * z + x; //0
				indices[index + 1] = (width + 1) * (z + 1) + x; //1
				indices[index + 2] = (width + 1) * z + x + 1; //2

				indices[index + 3] = (width + 1) * z + x + 1; //2
				indices[index + 4] = (width + 1) * (z + 1) + x; //1
				indices[index + 5] = (width + 1) * (z + 1) + x + 1; //1

				index += 6;
			}
		}//for(z)
	}
}

void MapToolData::CreateNormalData()
{
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		D3DXVECTOR3 d1 = vertices[index1].Position - vertices[index0].Position;
		D3DXVECTOR3 d2 = vertices[index2].Position - vertices[index0].Position;

		D3DXVECTOR3 normal;
		D3DXVec3Cross(&normal, &d1, &d2);

		vertices[index0].Normal += normal;
		vertices[index1].Normal += normal;
		vertices[index2].Normal += normal;

		D3DXVec3Normalize(&vertices[index0].Normal, &vertices[index0].Normal);
		D3DXVec3Normalize(&vertices[index1].Normal, &vertices[index1].Normal);
		D3DXVec3Normalize(&vertices[index2].Normal, &vertices[index2].Normal);
	}
}

void MapToolData::CreateTangentData()
{
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		//ÅºÁ¨Æ®»ý¼º
		D3DXVECTOR3 p0 = vertices[index0].Position;
		D3DXVECTOR3 p1 = vertices[index1].Position;
		D3DXVECTOR3 p2 = vertices[index2].Position;

		D3DXVECTOR3 uv0 = vertices[index0].Uv;
		D3DXVECTOR3 uv1 = vertices[index1].Uv;
		D3DXVECTOR3 uv2 = vertices[index2].Uv;


		// Á¤±Ô Á÷±³
		D3DXVECTOR3 e0 = vertices[index1].Position - vertices[index0].Position;
		D3DXVECTOR3 e1 = vertices[index2].Position - vertices[index0].Position;

		float u0 = vertices[index1].Uv.x - vertices[index0].Uv.x;
		float u1 = vertices[index2].Uv.x - vertices[index0].Uv.x;
		float v0 = vertices[index1].Uv.y - vertices[index0].Uv.y;
		float v1 = vertices[index2].Uv.y - vertices[index0].Uv.y;
		float r = 1.0f / (u0 * v1 - v0 * u1);

		// tangent º¤ÅÍ
		D3DXVECTOR3 tangent;
		tangent.x = r * (v1 * e0.x - v0 * e1.x);
		tangent.y = r * (v1 * e0.y - v0 * e1.y);
		tangent.z = r * (v1 * e0.z - v0 * e1.z);

		vertices[index0].Tangent += tangent;
		vertices[index1].Tangent += tangent;
		vertices[index2].Tangent += tangent;
	}

	for (UINT i = 0; i < vertexCount; i++)
	{
		D3DXVECTOR3 temp = (vertices[i].Tangent - vertices[i].Normal * D3DXVec3Dot(&vertices[i].Normal, &vertices[i].Tangent));
		D3DXVec3Normalize(&vertices[i].Tangent, &temp);
	}
}