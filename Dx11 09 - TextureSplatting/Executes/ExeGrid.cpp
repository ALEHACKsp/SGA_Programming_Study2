#include "stdafx.h"
#include "ExeGrid.h"

ExeGrid::ExeGrid(ExecuteValues * values)
	:Execute(values)
{
	// loadInfo �̹��� �ҷ��� �� ���°� �׳� �о���� 256���� ������(���������ϱ����ؼ�)
	Texture* heightMap = new Texture(Contents + L"HeightMaps/HeightMap.png");
	vector<D3DXCOLOR> heights;
	heightMap->ReadPixels(DXGI_FORMAT_R8G8B8A8_UNORM, &heights);

	width = heightMap->GetWidth() - 1; // ���� ���� ������ -1 �� ��
	height = heightMap->GetHeight() - 1; // ���� ���� ������ -1 �� ��

	shader = new Shader(Shaders + L"007_TextureSplatting.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;

	texture[0] = new Texture(Contents + L"Textures/Dirt.png");
	texture[1] = new Texture(Contents + L"Textures/Wall.png");
	texture[2] = new Texture(Contents + L"HeightMaps/AlphaMap.png");

	// Create VertexData
	{
		vertexCount = (width + 1) * (height + 1);
		vertices = new VertexType[vertexCount];

		// z������ �ϴ� ���� 3d��
		for (UINT z = 0; z <= height; z++) {
			for (UINT x = 0; x <= width; x++) {
				UINT index = (width + 1) * z + x;

				vertices[index].Position.x = (float)x;
				// �������� r���� �� �׳� ���� �ʹ� �������� ���Ƿ� �����ذ�
				vertices[index].Position.y = (float)(heights[index].r * 255.0f) / 7.5f;
				vertices[index].Position.z = (float)z;

				vertices[index].Uv.x = (float)x / width;
				vertices[index].Uv.y = (float)z / height;
			}
		} // for(z) 
	}

	// Create IndexData
	{
		indexCount = width * height * 6;
		indices = new UINT[indexCount];

		UINT index = 0;
		for (UINT z = 0; z < height; z++) {
			for (UINT x = 0; x < width; x++) {
				// 0�� index ���� �ϴ�
				indices[index + 0] = (width + 1) * z + x; 
				// 1�� index ���� ���
				indices[index + 1] = (width + 1) * (z + 1) + x; 
				// 2�� index ���� �ϴ�
				indices[index + 2] = (width + 1) * z + x + 1; 
			
				// 2�� index ���� �ϴ�
				indices[index + 3] = (width + 1) * z + x + 1; 
				// 1�� index ���� ���
				indices[index + 4] = (width + 1) * (z + 1) + x; 
				// 3�� index ���� ���
				indices[index + 5] = (width + 1) * (z + 1) + x + 1; 

				index += 6;
			}
		} // for(z)
	}

	// CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // ��� ��������� ���� ����
		desc.ByteWidth = sizeof(VertexType) * vertexCount; // ���� ���ۿ� �� �������� ũ��
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // �긦 ���ؼ� ���� �� lock ���
		data.pSysMem = vertices; // �� �������� �ּ�

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr)); // �����Ǹ� hr 0���� ū �� �Ѿ��
	}

	// CreateIndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // ��� ��������� ���� ����
		desc.ByteWidth = sizeof(UINT) * indexCount; // ���� ���ۿ� �� �������� ũ��
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // �긦 ���ؼ� ���� �� lock ���
		data.pSysMem = indices; // �� �������� �ּ�

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr)); // �����Ǹ� hr 0���� ū �� �Ѿ��
	}
}

ExeGrid::~ExeGrid()
{
	// dx�κ��� ��������� release
	// ���� �������� ��������
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);

	for (int i = 0; i < 3; i++)
		SAFE_DELETE(texture[i]);
}

void ExeGrid::Update()
{

}

void ExeGrid::PreRender()
{
}

void ExeGrid::Render()
{
	UINT stride = sizeof(VertexType); // �׸� ũ��
	UINT offset = 0;

	// vertex buffer ������ �� �� ����
	// IA �ٴ� ���� ���߿� �������ֽǲ�
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// �׸� ��� ����
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	texture[0]->SetShaderResource(0);
	texture[1]->SetShaderResource(1);
	texture[2]->SetShaderResource(2);

	colorBuffer->SetPSBuffer(0); // �������� ��ȣ
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	// ������ �׸��� ��
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

void ExeGrid::PostRender()
{

}

void ExeGrid::ResizeScreen()
{
}
