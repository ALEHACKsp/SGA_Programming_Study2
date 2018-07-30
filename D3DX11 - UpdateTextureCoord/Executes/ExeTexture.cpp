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
		// dx11 설정은 desc로 함
		D3D11_SAMPLER_DESC desc;
		// 이 함수 호출시 기본값들이 세팅되서 넘어옴
		States::GetSamplerDesc(&desc);

		//CD3D11_SAMPLER_DESC desc2; // 이것도 자동으로 세팅해서 값으로 넘어옴
		//// 안쓰는 이유 틀어지는 값들이 은근히 많아서 따로 만드신거

		// 기본값 WRAP
		States::CreateSampler(&desc, &state[0]);

		//// CLAMP 도장처럼 찍는거
		//desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		//desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		//States::CreateSampler(&desc, &state[1]);

		//// MIRROR (복사랑 다르게 뒤집힘 u,v 값에 맞게 뒤집힘)
		//desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		//desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		//States::CreateSampler(&desc, &state[2]);

		//// BORDER 특정색으로 채우는거
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
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(VertexTexture) * vertexCount; // 정점 버퍼에 들어갈 데이터의 크기
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
		data.pSysMem = vertices; // 쓸 데이터의 주소

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	}

	// CreateIndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(UINT) * indexCount; // 정점 버퍼에 들어갈 데이터의 크기
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
		data.pSysMem = indices; // 쓸 데이터의 주소

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	}

	// Load SRV
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
			D3D::GetDevice(),
			(Textures + L"Box.png").c_str(),
			NULL, // 읽어드릴때 설정값 NULL이면 기본값
			NULL,
			&srv,
			NULL // 여기서 hr 리턴 받아도됨
		);
		assert(SUCCEEDED(hr));
	}

	// Load SRV2
	{
		HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
			D3D::GetDevice(),
			(Textures + L"Wall.png").c_str(),
			NULL, // 읽어드릴때 설정값 NULL이면 기본값
			NULL,
			&srv2,
			NULL // 여기서 hr 리턴 받아도됨
		);
		assert(SUCCEEDED(hr));
	}
}

ExeTexture::~ExeTexture()
{
	// dx로부터 만들어진건 release
	// 보통 역순으로 릴리즈함
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
	UINT stride = sizeof(VertexTexture); // 그릴 크기
	UINT offset = 0;

	// vertex buffer 여러개 들어갈 수 있음
	// IA 붙는 이유 나중에 설명해주실꺼
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// 그릴 방식 설정
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	colorBuffer->SetPSBuffer(0); // 레지스터 번호
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	D3D::GetDC()->PSSetSamplers(0, 1, &state[number]);
	// 픽셀 쉐이더에 텍스처 넣는거
	D3D::GetDC()->PSSetShaderResources(
		0, // 0번
		1, // 갯수 
		&srv);
	D3D::GetDC()->PSSetShaderResources(1, 1, &srv2);

	// 실제로 그리는 거
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

void ExeTexture::PostRender()
{
	// 창 만들 수 있음
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
