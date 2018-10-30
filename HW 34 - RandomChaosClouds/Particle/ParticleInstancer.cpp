#include "stdafx.h"
#include "ParticleInstancer.h"

ParticleInstancer::ParticleInstancer()
	:position(D3DXVECTOR3(0,0,0)), scale(D3DXVECTOR3(1,1,1))
{
	D3DXQuaternionIdentity(&rotation);
	size = D3DXVECTOR2(1, 1) * 0.5f;
	D3DXMatrixIdentity(&world);

	blendState[0] = new BlendState();
	// addtive
	blendState[1] = new BlendState();

	depthState[0] = new DepthStencilState();
	// None
	depthState[1] = new DepthStencilState();

	vertexCount = 0;

	// shader

}

ParticleInstancer::~ParticleInstancer()
{
	SAFE_DELETE(shader);

	SAFE_DELETE(blendState[0]);
	SAFE_DELETE(blendState[1]);

	SAFE_DELETE(depthState[0]);
	SAFE_DELETE(depthState[1]);
}

void ParticleInstancer::Init()
{
	list<D3DXVECTOR2> t;

	// Build Vertex Elements
	vector<VertexTexture> vertices;
	vector<int> indices;
	D3DXVECTOR2 uv = D3DXVECTOR2(0, 0);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);

	VertexTexture vertex;
	for (int d = 0; d < 4; d++)
	{
		switch (d)
		{
		case 0: uv = D3DXVECTOR2(1, 1); break;
		case 1: uv = D3DXVECTOR2(0, 1); break;
		case 2: uv = D3DXVECTOR2(0, 0); break;
		case 3: uv = D3DXVECTOR2(1, 0); break;
		}

		vertex.Position = D3DXVECTOR3(0, 0, 0);
		vertex.Uv = uv;

		vertices.push_back(vertex);
	}

	vertexCount = vertices.size();
	
	HRESULT hr;
	D3D11_BUFFER_DESC desc;
	D3D11_SUBRESOURCE_DATA data;

	// Vertex Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * vertices.size();
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &vertices[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	// Index Buffer
	{
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indices.size();
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		ZeroMemory(&data, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = &indices[0];

		hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}

}

void ParticleInstancer::Render()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationQuaternion(&R, &rotation);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	world = S * R * T;

	if (instanceTransformMatrices.size() == 0)
		return;

	// If we have more instances than room in our vertex buffer, grow it to the necessary size
	if ((instanceBuffer == NULL) || (instanceTransformMatrices.size() != instanceCount))
		CalcVertexBuffer();

	UINT strides[2];
	UINT offsets[2];
	ID3D11Buffer* buffers[2];

	buffers[0] = vertexBuffer;
	buffers[1] = instanceBuffer;

	strides[0] = sizeof(VertexTexture);
	strides[1] = sizeof(VertexDeclaration);

	offsets[0] = offsets[1] = offsets[2];

	D3D::GetDC()->IASetVertexBuffers(0, 2, buffers, strides, offsets);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->DrawIndexedInstanced()
}

void ParticleInstancer::CalcVertexBuffer()
{
	if (instanceBuffer != NULL)
		SAFE_RELEASE(instanceBuffer);

	instanceCount = instanceTransformMatrices.size();
	VertexDeclaration* declarations = new VertexDeclaration[instanceCount];

	for (int i = 0; i < instanceCount; i++)
		declarations->id = i;

	// CreateInstanceBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(VertexDeclaration) * instanceCount; // 정점 버퍼에 들어갈 데이터의 크기
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
		data.pSysMem = declarations; // 쓸 데이터의 주소
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &instanceBuffer);
		assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	}
}

void ParticleInstancer::TranslateOO(D3DXVECTOR3 distance)
{
}

void ParticleInstancer::TranslateAA(D3DXVECTOR3 distance)
{
}

void ParticleInstancer::Rotate(D3DXVECTOR3 axis, float angle)
{
}
