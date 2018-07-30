#include "stdafx.h"
#include "ExeTexture.h"

ExeTexture::ExeTexture(ExecuteValues * values)
	:Execute(values)
	, vertexCount(4), indexCount(6)
	, number(0)
{
	shader = new Shader(Shaders + L"005_TextureUV.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;

	vertices = new VertexTexture[vertexCount];
	vertices[0].Position = D3DXVECTOR3(0, 0, 0);
	vertices[1].Position = D3DXVECTOR3(0, 1, 0);
	vertices[2].Position = D3DXVECTOR3(1, 0, 0);
	vertices[3].Position = D3DXVECTOR3(1, 1, 0);

	vertices[0].Uv = D3DXVECTOR2(0, 1.0f);
	vertices[1].Uv = D3DXVECTOR2(0, 0);
	vertices[2].Uv = D3DXVECTOR2(1.0f, 1.0f);
	vertices[3].Uv = D3DXVECTOR2(1.0f, 0);

	indices = new UINT[indexCount]{ 0, 1, 2, 2, 1, 3 };

	{
		// dx11 ������ desc�� ��
		D3D11_SAMPLER_DESC desc;
		// �� �Լ� ȣ��� �⺻������ ���õǼ� �Ѿ��
		States::GetSamplerDesc(&desc);

		//CD3D11_SAMPLER_DESC desc2; // �̰͵� �ڵ����� �����ؼ� ������ �Ѿ��
		//// �Ⱦ��� ���� Ʋ������ ������ ������ ���Ƽ� ���� ����Ű�

		// �⺻�� WRAP
		States::CreateSampler(&desc, &state[0]);

		//// CLAMP ����ó�� ��°�
		//desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		//desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		//States::CreateSampler(&desc, &state[1]);

		//// MIRROR (����� �ٸ��� ������ u,v ���� �°� ������)
		//desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		//desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		//States::CreateSampler(&desc, &state[2]);

		//// BORDER Ư�������� ä��°�
		//desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		//desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		//
		//D3DXCOLOR color = D3DXCOLOR(1, 0, 0, 1);
		//memcpy(&desc.BorderColor, &color, sizeof(D3DXCOLOR));
		//States::CreateSampler(&desc, &state[3]);

		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		States::CreateSampler(&desc, &state[1]);
	}

	// CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // ��� ��������� ���� ����
		desc.ByteWidth = sizeof(VertexTexture) * vertexCount; // ���� ���ۿ� �� �������� ũ��
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

	// Load SRV
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
			D3D::GetDevice(),
			(Textures + L"Box.png").c_str(),
			NULL, // �о�帱�� ������ NULL�̸� �⺻��
			NULL,
			&srv,
			NULL // ���⼭ hr ���� �޾Ƶ���
		);
		assert(SUCCEEDED(hr));
	}

	// Load SRV2
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
			D3D::GetDevice(),
			(Textures + L"Wall.png").c_str(),
			NULL, // �о�帱�� ������ NULL�̸� �⺻��
			NULL,
			&srv2,
			NULL // ���⼭ hr ���� �޾Ƶ���
		);
		assert(SUCCEEDED(hr));
	}
}

ExeTexture::~ExeTexture()
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
}

void ExeTexture::Update()
{
}

void ExeTexture::PreRender()
{
}

void ExeTexture::Render()
{
	UINT stride = sizeof(VertexTexture); // �׸� ũ��
	UINT offset = 0;

	// vertex buffer ������ �� �� ����
	// IA �ٴ� ���� ���߿� �������ֽǲ�
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// �׸� ��� ����
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	colorBuffer->SetPSBuffer(0); // �������� ��ȣ
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	D3D::GetDC()->PSSetSamplers(0, 1, &state[number]);
	// �ȼ� ���̴��� �ؽ�ó �ִ°�
	D3D::GetDC()->PSSetShaderResources(
		0, // 0��
		1, // ���� 
		&srv);
	D3D::GetDC()->PSSetShaderResources(1, 1, &srv2);

	// ������ �׸��� ��
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

void ExeTexture::PostRender()
{
	// â ���� �� ����
	ImGui::Begin("Color");
	{
		//ImGui::SliderInt("Number", &number, 0, 3);
		ImGui::SliderInt("Number", &number, 0, 1);
		ImGui::SliderFloat3("Color", 
			(float *)&colorBuffer->Data.Color, 0, 1);
	}
	ImGui::End();
}

void ExeTexture::ResizeScreen()
{
}
