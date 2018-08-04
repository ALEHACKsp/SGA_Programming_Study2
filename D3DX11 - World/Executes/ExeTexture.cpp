#include "stdafx.h"
#include "ExeTexture.h"

ExeTexture::ExeTexture(ExecuteValues * values)
	:Execute(values)
	, vertexCount(4), indexCount(6)
	, number(0)
	, location(0, 0), rotation(0, 0, 0), scale(1, 1)
	, number2(0)
{
	shader = new Shader(Shaders + L"005_TextureUV.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;

	vertices = new VertexTexture[vertexCount];
	vertices[0].Position = D3DXVECTOR3(-0.5f, -0.5f, 0);
	vertices[1].Position = D3DXVECTOR3(-0.5f, 0.5f, 0);
	vertices[2].Position = D3DXVECTOR3(0.5f, -0.5f, 0);
	vertices[3].Position = D3DXVECTOR3(0.5f, 0.5f, 0);

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

	{
		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);
		// 기본형
		States::CreateRasterizer(&desc, &rasterizer[0]);

		//desc.CullMode = D3D11_CULL_NONE;
		desc.FillMode = D3D11_FILL_WIREFRAME;
		States::CreateRasterizer(&desc, &rasterizer[1]);
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

	SAFE_RELEASE(srv);
	SAFE_RELEASE(srv2);
}

void ExeTexture::Update()
{
	//D3DXMATRIX R;
	//D3DXMatrixIdentity(&R);

	D3DXMATRIX Y, Z;
	D3DXMatrixIdentity(&Y);
	D3DXMatrixIdentity(&Z);

	D3DXMATRIX T;
	D3DXMatrixIdentity(&T);

	if (Keyboard::Get()->Press(VK_SHIFT)) 
	{
		if (Keyboard::Get()->Press('A'))
			rotation.y -= 30.0f * Time::Delta();
		else if(Keyboard::Get()->Press('D'))
			rotation.y += 30.0f * Time::Delta();

		if (Keyboard::Get()->Press('W'))
			rotation.z -= 30.0f * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			rotation.z += 30.0f * Time::Delta();
	}
	else
	{
		if (Keyboard::Get()->Press('A'))
			location.x -= 3.0f * Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			location.x += 3.0f * Time::Delta();

		if (Keyboard::Get()->Press('W'))
			location.y += 3.0f * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			location.y -= 3.0f * Time::Delta();
	}

	D3DXMatrixTranslation(&T, location.x, location.y, 0);

	D3DXMatrixRotationY(&Y, Math::ToRadian(rotation.y));
	D3DXMatrixRotationZ(&Z, Math::ToRadian(rotation.z));

	//R = Y * Z;

	//world = R;
	world = Y * Z * T;
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

	// 레스터라이저 세팅
	D3D::GetDC()->RSSetState(rasterizer[number2]);

	//D3DXMATRIX world, S, R, T;
	//D3DXMatrixIdentity(&world); // identity 해줘야함

	//// 위치 이동
	////world._41 = 3; // x축으로 3만큼 이동
	////world._42 = 0;
	////world._43 = 0;

	//// 같은 방법 이 녀석 쓸때는 identity 안해도 됨
	//// D3DXMatrixTranslation(&world, 3, 0, 0);

	//// 둘다 하고 싶어서 이렇게 하면 마지막줄만 적용됨
	//D3DXMatrixScaling(&S, 2, 3, 1);
	//D3DXMatrixRotationY(&R, Math::ToRadian(45));
	////D3DXMatrixTranslation(&T, 1.5f, 0, 0);

	//world = R;

	////world = S * T;
	////world = T * S;
	//// 순서 차이 있음 -> SRT 순으로 알고 있자

	worldBuffer->SetMatrix(world);
	worldBuffer->SetVSBuffer(1);

	// 실제로 그리는 거
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);


	//D3DXMATRIX world2;
	//D3DXMatrixIdentity(&world2); // identity 해줘야함

	//world2._41 = 1; // x축으로 1만큼 이동
	//world2._42 = 0;
	//world2._43 = 0;

	//worldBuffer->SetMatrix(world2);
	//worldBuffer->SetVSBuffer(1);

	//// 실제로 그리는 거
	//D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

void ExeTexture::PostRender()
{
	// 창 만들 수 있음
	ImGui::Begin("Color");
	{
		//ImGui::SliderInt("Number", &number, 0, 3);
		ImGui::SliderInt("Number", &number, 0, 1);
		ImGui::SliderInt("Number2", &number2, 0, 1);
		ImGui::SliderFloat3("Color", 
			(float *)&colorBuffer->Data.Color, 0, 1);
	}
	ImGui::End();
}

void ExeTexture::ResizeScreen()
{
}
