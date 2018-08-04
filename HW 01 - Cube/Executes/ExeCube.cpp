#include "stdafx.h"
#include "ExeCube.h"

ExeCube::ExeCube(ExecuteValues * values)
	:Execute(values)
	, vertexCount(6 * 4), indexCount(6 * 6)
	, location(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1)
	, ratioAll(0.5f), isAllRatio(false)
	//srv(NULL), srv2(NULL)
{
	shader = new Shader(Shaders + L"/Homework/" + L"TextureUV.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;
	uvBuffer = new UVBuffer;

	vertices = new VertexTexture[vertexCount];

	float pos[][3] =
	{
		// 앞
		{ -0.5f,-0.5f,-0.5f },{ -0.5f, 0.5f,-0.5f },{ 0.5f,-0.5f,-0.5f },{ 0.5f, 0.5f,-0.5f },
		// 뒤 z값 반대					 
		{ -0.5f,-0.5f, 0.5f },{ -0.5f, 0.5f, 0.5f },{ 0.5f,-0.5f, 0.5f },{ 0.5f, 0.5f, 0.5f },
		// 좌							   
		{ -0.5f,-0.5f, 0.5f },{ -0.5f, 0.5f, 0.5f },{-0.5f,-0.5f,-0.5f },{-0.5f, 0.5f,-0.5f },
		// 우 x값 반대					 
		{  0.5f,-0.5f, 0.5f },{  0.5f, 0.5f, 0.5f },{ 0.5f,-0.5f,-0.5f },{ 0.5f, 0.5f,-0.5f },
		// 위 							   
		{ -0.5f, 0.5f,-0.5f },{ -0.5f, 0.5f, 0.5f },{ 0.5f, 0.5f,-0.5f },{ 0.5f, 0.5f, 0.5f },
		// 아래 y값 반대				   
		{ -0.5f,-0.5f,-0.5f },{ -0.5f,-0.5f, 0.5f },{ 0.5f,-0.5f,-0.5f },{ 0.5f,-0.5f, 0.5f },
	};

	for (int i = 0; i < 6; i++) {
		vertices[i * 4 + 0].Position = D3DXVECTOR3(pos[i * 4 + 0][0], pos[i * 4 + 0][1], pos[i * 4 + 0][2]);
		vertices[i * 4 + 1].Position = D3DXVECTOR3(pos[i * 4 + 1][0], pos[i * 4 + 1][1], pos[i * 4 + 1][2]);
		vertices[i * 4 + 2].Position = D3DXVECTOR3(pos[i * 4 + 2][0], pos[i * 4 + 2][1], pos[i * 4 + 2][2]);
		vertices[i * 4 + 3].Position = D3DXVECTOR3(pos[i * 4 + 3][0], pos[i * 4 + 3][1], pos[i * 4 + 3][2]);

		vertices[i * 4 + 0].Uv = D3DXVECTOR2(0, 1.0f);
		vertices[i * 4 + 1].Uv = D3DXVECTOR2(0, 0);
		vertices[i * 4 + 2].Uv = D3DXVECTOR2(1.0f, 1.0f);
		vertices[i * 4 + 3].Uv = D3DXVECTOR2(1.0f, 0);
	}

	//indices = new UINT[indexCount]{ 0, 1, 2, 2, 1, 3 };
	indices = new UINT[indexCount]; 
	for (int i = 0; i < 6; i++) {
		// 반시계
		if (i % 2 == 0) {
			indices[i * 6 + 0] = i * 4 + 0;
			indices[i * 6 + 1] = i * 4 + 1;
			indices[i * 6 + 2] = i * 4 + 2;
			indices[i * 6 + 3] = i * 4 + 2;
			indices[i * 6 + 4] = i * 4 + 1;
			indices[i * 6 + 5] = i * 4 + 3;
		}
		// 시계방향으로 그려야함
		else {
			indices[i * 6 + 0] = i * 4 + 1;
			indices[i * 6 + 1] = i * 4 + 0;
			indices[i * 6 + 2] = i * 4 + 2;
			indices[i * 6 + 3] = i * 4 + 1;
			indices[i * 6 + 4] = i * 4 + 2;
			indices[i * 6 + 5] = i * 4 + 3;
		}
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
		// 초기 SRV 세팅
		for (int i = 0; i < 12; i++)
			srv[i] = NULL;

		//for (int i = 0; i < 6; i++) {
		//	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
		//		D3D::GetDevice(),
		//		(Textures + L"Homework/" + 
		//			L"dice" + L"_" + to_wstring(i+1) + L".png").c_str(),
		//		NULL, // 읽어드릴때 설정값 NULL이면 기본값
		//		NULL,
		//		&dice[i],
		//		NULL // 여기서 hr 리턴 받아도됨
		//	);
		//	assert(SUCCEEDED(hr));

		//	srv[i * 2 + 0] = dice[i];
		//	srv[i * 2 + 1] = dice[i];
		//}

		temp = bind(&ExeCube::CreateSRV, this, placeholders::_1);

		//HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
		//	D3D::GetDevice(),
		//	(Textures + L"Box.png").c_str(),
		//	NULL, // 읽어드릴때 설정값 NULL이면 기본값
		//	NULL,
		//	&srv,
		//	NULL // 여기서 hr 리턴 받아도됨
		//);
		//assert(SUCCEEDED(hr));
	}

	for (int i = 0; i < 6; i++)
		ratio[i] = 0.5f;
}

ExeCube::~ExeCube()
{
	// dx로부터 만들어진건 release
	// 보통 역순으로 릴리즈함
	SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);
	SAFE_DELETE(uvBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);

	for (int i = 0; i < 12; i++)
		SAFE_RELEASE(srv[i]);
}

void ExeCube::Update()
{
	D3DXMATRIX S;
	D3DXMatrixIdentity(&S);

	D3DXMATRIX R;
	D3DXMatrixIdentity(&R);

	D3DXMATRIX X, Y, Z;
	D3DXMatrixIdentity(&X);
	D3DXMatrixIdentity(&Y);
	D3DXMatrixIdentity(&Z);

	D3DXMATRIX T;
	D3DXMatrixIdentity(&T);

	if (Keyboard::Get()->Press(VK_SHIFT))
	{
		if (Keyboard::Get()->Press('A'))
			location.x -= 3.0f * Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			location.x += 3.0f * Time::Delta();

		if (Keyboard::Get()->Press('W'))
			location.y += 3.0f * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			location.y -= 3.0f * Time::Delta();

		if (Keyboard::Get()->Press('Q'))
			location.z -= 3.0f * Time::Delta();
		else if (Keyboard::Get()->Press('E'))
			location.z += 3.0f * Time::Delta();
	}
	else if (Keyboard::Get()->Press(VK_CONTROL))
	{
		if (Keyboard::Get()->Press('A'))
			scale.x -= 1.0f * Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			scale.x += 1.0f * Time::Delta();

		if (Keyboard::Get()->Press('W'))
			scale.y += 1.0f * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			scale.y -= 1.0f * Time::Delta();

		if (Keyboard::Get()->Press('Q'))
			scale.z -= 1.0f * Time::Delta();
		else if (Keyboard::Get()->Press('E'))
			scale.z += 1.0f * Time::Delta();
	}
	else 
	{
		if (Keyboard::Get()->Press('W'))
			rotation.x -= 30.0f * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			rotation.x += 30.0f * Time::Delta();

		if (Keyboard::Get()->Press('A'))
			rotation.y -= 30.0f * Time::Delta();
		else if (Keyboard::Get()->Press('D'))
			rotation.y += 30.0f * Time::Delta();

		if (Keyboard::Get()->Press('Q'))
			rotation.z -= 30.0f * Time::Delta();
		else if (Keyboard::Get()->Press('E'))
			rotation.z += 30.0f * Time::Delta();
	}

	//if (Keyboard::Get()->Press(VK_LEFT)) {
	//	if (uvBuffer->Data.ratio < 1.0f)
	//		uvBuffer->Data.ratio -= 0.3f * Time::Delta();
	//	if (uvBuffer->Data.ratio < 0)
	//		uvBuffer->Data.ratio = 0;
	//}
	//if(Keyboard::Get()->Press(VK_RIGHT))
	//{
	//	if (uvBuffer->Data.ratio > 0)
	//		uvBuffer->Data.ratio += 0.3f * Time::Delta();
	//	if (uvBuffer->Data.ratio > 1.0f)
	//		uvBuffer->Data.ratio = 1.0f;
	//}

	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);

	D3DXMatrixTranslation(&T, location.x, location.y, location.z);

	D3DXMatrixRotationX(&X, Math::ToRadian(rotation.x));
	D3DXMatrixRotationY(&Y, Math::ToRadian(rotation.y));
	D3DXMatrixRotationZ(&Z, Math::ToRadian(rotation.z));

	R = X * Y * Z;

	world = S * R * T;
}

void ExeCube::PreRender()
{
}

void ExeCube::Render()
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
	if (isAllRatio) {
		uvBuffer->Data.ratio = ratioAll;
		uvBuffer->SetPSBuffer(1);
	}
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	//// 픽셀 쉐이더에 텍스처 넣는거
	//D3D::GetDC()->PSSetShaderResources(0, 1, &srv);
	//D3D::GetDC()->PSSetShaderResources(1, 1, &srv2);

	worldBuffer->SetMatrix(world);
	worldBuffer->SetVSBuffer(1);

	// 실제로 그리는 거
	//D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	for (int i = 0; i < 6; i++) {
		D3D::GetDC()->PSSetShaderResources(0, 1, &srv[i*2 + 0]);
		D3D::GetDC()->PSSetShaderResources(1, 1, &srv[i*2 + 1]);

		if (!isAllRatio) {
			uvBuffer->Data.ratio = ratio[i];
			uvBuffer->SetPSBuffer(1);
		}

		D3D::GetDC()->DrawIndexed(6, i * 6 + 0, 0);
	}
}

void ExeCube::PostRender()
{
	string str[] = { "Front", "Back", "Left", "Right", "Top", "Bottom" };

	// 창 만들 수 있음
	ImGui::Begin("Texture");
	{
		if (ImGui::SliderFloat("Tex Ratio All", &ratioAll, 0.0f, 1.0f))
			isAllRatio = true;

		for (int i = 0; i < 6; i++) {
			// ImVec2 안써주면 글자 크기만 하면 됨
			if (ImGui::Button((str[i] + " Tex1 Select").c_str())) {
				this->SetSRVNum(i * 2 + 0);
				Path::OpenFileDialog(L"Box.png", 
					Path::ImageFilter, // 어떤 확장자 이미지 쓸지
					Textures, // 선생님이 만드신 텍스처 기본 주소
					temp);
			}

			if (ImGui::Button((str[i] + " Tex2 Select").c_str())) {
				this->SetSRVNum(i * 2 + 1);
				Path::OpenFileDialog(L"Wall.png", Path::ImageFilter,
					Textures, temp);
			}

			if (srv[i * 2 + 0] != NULL &&
				srv[i * 2 + 1] != NULL &&
				ImGui::SliderFloat((str[i] + " Tex Ratio").c_str(),
					&ratio[i], 0.0f, 1.0f)) {
				isAllRatio = false;
			}
		}
	}
	ImGui::End();
}

void ExeCube::ResizeScreen()
{
}

void ExeCube::CreateSRV(wstring str)
{
	HRESULT hr = D3DX11CreateShaderResourceViewFromFile(
		D3D::GetDevice(),
		str.c_str(),
		NULL, // 읽어드릴때 설정값 NULL이면 기본값
		NULL,
		&srv[srvNum],
		//&srv,
		NULL // 여기서 hr 리턴 받아도됨
	);
	assert(SUCCEEDED(hr));
}