#include "stdafx.h"
#include "Terrain.h"

#include "../Objects/GameModel.h"

Terrain::Terrain(Material * material, wstring heightMap)
	: material(material)
{
	heightTexture = new Texture(heightMap);

	worldBuffer = new WorldBuffer();

	CreateData();
	CreateBuffer();

	{
		fillModeNumber = 1;

		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);

		States::CreateRasterizer(&desc, &fillMode[0]);

		desc.FillMode = D3D11_FILL_WIREFRAME;
		States::CreateRasterizer(&desc, &fillMode[1]);
	}
}

Terrain::~Terrain()
{
	SAFE_DELETE(material);

	SAFE_DELETE(heightTexture);
	SAFE_DELETE(worldBuffer);

	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(fillMode[i]);
}

void Terrain::Update()
{
}

void Terrain::Render()
{
	UINT stride = sizeof(VertexTextureNormal);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	D3D::GetDC()->RSSetState(fillMode[fillModeNumber]);

	worldBuffer->SetVSBuffer(1);
	material->PSSetBuffer();

	D3D::GetDC()->DrawIndexed(indices.size(), 0, 0);
}

void Terrain::PostRender()
{
	ImGui::SliderInt("Wireframe", &fillModeNumber, 0, 1);
}

void Terrain::UpdateModel(GameModel * model)
{
	//// 높이값 변경
	//{
	//	D3DXVECTOR3 position = model->Position();

	//	// 충돌 계산 방법
	//	D3DXVECTOR3 newPosition;
	//	if (Y(&newPosition, position) == true)
	//		position.y = newPosition.y;
	//	model->Position(position);
	//}

	// 높이값 변경 및 회전
	{
		D3DXVECTOR3 position = model->Position();
		D3DXVECTOR3 right;
		right = model->Right();

		D3DXVECTOR3 newPosition;
		D3DXVECTOR3 newForward, newUp;
		if (Y(&newPosition, &newForward, &newUp, position, right) == true)
		{
			position.y = newPosition.y;
		}

		D3DXVec3Cross(&right, &newForward, &newUp);

		D3DXVec3Normalize(&newUp, &newUp);
		D3DXVec3Normalize(&newForward, &newForward);
		D3DXVec3Normalize(&right, &right);

		model->Up(newUp);
		model->Direction(-newForward);
		model->Right(right);
		model->Position(position);
	}
}

float Terrain::Y(D3DXVECTOR3& position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x >= width) return 0.0f;
	if (z < 0 || z >= height) return 0.0f;

	UINT index[4];

	index[0] = (width + 1) * z + x;
	index[1] = (width + 1) * (z + 1) + x;
	index[2] = (width + 1) * z + (x + 1);
	index[3] = (width + 1) * (z + 1) + (x + 1);

	D3DXVECTOR3 v[4];
	for (int i = 0; i < 4; i++)
		v[i] = vertices[index[i]].Position;

	// 길이에 대한 비율
	float ddx = (position.x - v[0].x) / 1.0f;
	float ddz = (position.z - v[0].z) / 1.0f;

	D3DXVECTOR3 temp;

	// 밑면
	if (ddx + ddz <= 1)
	{
		temp = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz;
	}
	// 윗면
	{
		ddx = 1 - ddx;
		ddz = 1 - ddz;

		temp = v[3] + (v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz;
	}

	return temp.y;
}

bool Terrain::Y(OUT D3DXVECTOR3 * out, D3DXVECTOR3 & position)
{
	D3DXVECTOR3 start = D3DXVECTOR3(position.x, 1000, position.z);
	D3DXVECTOR3 direction = D3DXVECTOR3(0, -1, 0);

	// 삼각형 전체 충돌하는 방법 하나씩 충돌하는 방법 두가지가있음
	// 정밀하게 하려면 전체 삼각형에 대해 돌아야함

	// 원래 for문 전체 돌아야함
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x >= width) return false;
	if (z < 0 || z >= height) return false;

	UINT index[4];
	index[0] = (width + 1) * z + x;
	index[1] = (width + 1) * (z + 1) + x;
	index[2] = (width + 1) * z + (x + 1);
	index[3] = (width + 1) * (z + 1) + (x + 1);

	D3DXVECTOR3 p[4];
	for (int i = 0; i < 4; i++)
		p[i] = vertices[index[i]].Position;

	float u, v, distance;
	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start,
		&direction, &u, &v, &distance))
	{
		*out = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

		return true;
	}

	if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start,
		&direction, &u, &v, &distance))
	{
		*out = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;

		return true;
	}

	return false;
}

bool Terrain::Y(OUT D3DXVECTOR3 * outPos, OUT D3DXVECTOR3 * outForward, OUT D3DXVECTOR3 * outUp, D3DXVECTOR3 & position, D3DXVECTOR3 & right)
{
	D3DXVECTOR3 start = D3DXVECTOR3(position.x, 1000, position.z);
	D3DXVECTOR3 direction = D3DXVECTOR3(0, -1, 0);

	// 삼각형 전체 충돌하는 방법 하나씩 충돌하는 방법 두가지가있음
	// 정밀하게 하려면 전체 삼각형에 대해 돌아야함

	// 원래 for문 전체 돌아야함
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x < 0 || x >= width) return false;
	if (z < 0 || z >= height) return false;

	// 간격
	UINT stride = 3;

	UINT index[4];
	// 간격이 넘어버리면 한 칸으로 
	if (z + stride >= height && x + stride >= width)
	{
		index[0] = (width + 1) * z + x;
		index[1] = (width + 1) * (z + 1) + x;
		index[2] = (width + 1) * z + (x + 1);
		index[3] = (width + 1) * (z + 1) + (x + 1);
	}
	else 
	{
		index[0] = (width + 1) * z + x;
		index[1] = (width + 1) * (z + stride) + x;
		index[2] = (width + 1) * z + (x + stride);
		index[3] = (width + 1) * (z + stride) + (x + stride);
	}

	D3DXVECTOR3 p[4], n[4];
	for (int i = 0; i < 4; i++) {
		p[i] = vertices[index[i]].Position;
		n[i] = vertices[index[i]].Normal;
	}

	float u, v, distance;

	D3DXVECTOR3 newForward;
	D3DXVECTOR3 up;

	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start,
		&direction, &u, &v, &distance))
	{
		*outPos = p[0] + (p[1] - p[0]) * u + (p[2] - p[0]) * v;

		up = (n[0] + n[1] + n[2]) / 3;
		//D3DXVec3Normalize(&up, &up);
		up = n[0];

		D3DXVec3Cross(&newForward, &up, &right);

		*outForward = newForward;
		*outUp = up;

		return true;
	}

	if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start,
		&direction, &u, &v, &distance))
	{
		*outPos = p[3] + (p[1] - p[3]) * u + (p[2] - p[3]) * v;

		up = (n[3] + n[1] + n[2]) / 3;
		//D3DXVec3Normalize(&up, &up);
		up = n[3];

		D3DXVec3Cross(&newForward, &up, &right);

		*outForward = newForward;
		*outUp = up;

		return true;
	}

	return false;
}

void Terrain::CreateData()
{
	vector<D3DXCOLOR> heights;
	heightTexture->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	width = heightTexture->GetWidth() - 1;
	height = heightTexture->GetHeight() - 1;

	//Create VertexData
	{
		vertices.assign((width + 1) *(height + 1), VertexTextureNormal());

		for (UINT z = 0; z <= height; z++)
		{
			for (UINT x = 0; x <= width; x++)
			{
				UINT index = (width + 1) * z + x;

				vertices[index].Position.x = (float)x;
				vertices[index].Position.y = (float)(heights[index].r * 255.0f) / 10.0f;
				vertices[index].Position.z = (float)z;

				vertices[index].Uv.x = (float)x / width;
				vertices[index].Uv.y = (float)z / height;
			}
		}//for(z)
	}


	//CreateIndexData
	{
		// 기본 자료형 usinged int 도 생성자로 써도 됨 UINT() 이걸로
		indices.assign(width * height * 6, UINT());

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

	// CreateNormal
	{
		for (UINT i = 0; i < (indices.size() / 3); i++)
		{
			UINT index0 = indices[i * 3 + 0];
			UINT index1 = indices[i * 3 + 1];
			UINT index2 = indices[i * 3 + 2];

			VertexTextureNormal v0 = vertices[index0];
			VertexTextureNormal v1 = vertices[index1];
			VertexTextureNormal v2 = vertices[index2];

			D3DXVECTOR3 d1 = v1.Position - v0.Position;
			D3DXVECTOR3 d2 = v2.Position - v0.Position;

			D3DXVECTOR3 normal;
			D3DXVec3Cross(&normal, &d1, &d2);

			vertices[index0].Normal += normal;
			vertices[index1].Normal += normal;
			vertices[index2].Normal += normal;
		}

		for (UINT i = 0; i < vertices.size(); i++)
			D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
	}
}

void Terrain::CreateBuffer()
{
	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTextureNormal) * vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		// &vertices 이건 객체의 주소 0번째거를 넣어야함
		data.pSysMem = &vertices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = &indices[0];

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}


