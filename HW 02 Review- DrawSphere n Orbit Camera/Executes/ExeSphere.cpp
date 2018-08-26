#include "stdafx.h"
#include "ExeSphere.h"

#if CASE
ExeSphere::ExeSphere(ExecuteValues * values)
	:Execute(values)
	, radius(5.0f), stackCount(20), sliceCount(20)
#else
ExeSphere::ExeSphere(ExecuteValues* values, D3DXVECTOR3* basePos,
	float scale, float rotateSpeed,
	bool isTransduction, float dist, D3DXCOLOR color)
	: Execute(values)
	, radius(5.0f), stackCount(5), sliceCount(5)
	, basePos(basePos), scale(scale), rotateSpeed(rotateSpeed)
	, isTransduction(isTransduction), dist(dist)
	, pos(*basePos), deltaRadian(0)
#endif
{
	shader = new Shader(Shaders + L"003_Color.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;

	float phiStep = D3DX_PI / stackCount;
	float thetaStep = 2.0f * D3DX_PI / sliceCount;

	// Create VertexData
	{
		vertexCount = 2 + (sliceCount + 1) * (stackCount - 1);
		vertices = new VertexColor[vertexCount];
		
		UINT index = 0;
		for (UINT i = 0; i <= stackCount; i++) {
			// 위와 밑의 꼭지점 처리
			if (i == 0 || i == stackCount) {
				vertices[index].Position = D3DXVECTOR3(0, i == 0 ? radius : -radius, 0);
#if CASE
				vertices[index].Color = D3DXCOLOR(1, 1, 1, 1);
#else
				vertices[index].Color = color;
#endif
				index++;
			}
			else {
				float phi = i * phiStep;
				for (UINT j = 0; j <= sliceCount; j++) {
					float theta = j * thetaStep;
					vertices[index].Position = D3DXVECTOR3(
						(radius * sinf(phi) * cosf(theta)),
						(radius * cosf(phi)),
						(radius * sinf(phi) * sinf(theta)));
#if CASE
					vertices[index].Color = D3DXCOLOR(1, 1, 1, 1);
#else
					vertices[index].Color = color;
#endif
					index++;
				}
			}
		}
	}

	// Create IndexData
	{
		indexCount = (2 * 3 + (stackCount - 1) * 6) * sliceCount;
		indices = new UINT[indexCount];
		
		UINT index = 0;
		// 윗면
		for (UINT i = 1; i <= sliceCount; i++) {
			indices[index++] = 0;
			indices[index++] = i + 1;
			indices[index++] = i;
		}
		
		// 옆면
		UINT baseIndex = 1; // 위 꼭지점 빼려고
		UINT ringVertexCount = sliceCount + 1;
		// stackCount - 2 인 이유 정점 4개 써서 위 꼭지 아래꼭지 빼고 -1 해줘야함
		for (UINT i = 0; i < stackCount - 2; i++) {
			for (UINT j = 0; j < sliceCount; j++) {
				indices[index++] = baseIndex + i * ringVertexCount + j;
				indices[index++] = baseIndex + i * ringVertexCount + (j+1);
				indices[index++] = baseIndex + (i+1) * ringVertexCount + j;

				indices[index++] = baseIndex + (i+1) * ringVertexCount + j;
				indices[index++] = baseIndex + i * ringVertexCount + (j+1);
				indices[index++] = baseIndex + (i+1) * ringVertexCount + (j+1);
			}
		}

		// 아랫면
		UINT southPoleIndex = vertexCount - 1;
		baseIndex = southPoleIndex - ringVertexCount;
		for (UINT i = 0; i < sliceCount; i++) {
			indices[index++] = southPoleIndex;
			indices[index++] = baseIndex + i;
			indices[index++] = baseIndex + i + 1;
		}
	}

	// CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(VertexColor) * vertexCount; // 정점 버퍼에 들어갈 데이터의 크기
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

	// fillMode
	{
		fillModeNumber = 0;

		D3D11_RASTERIZER_DESC desc;
		States::GetRasterizerDesc(&desc);

		States::CreateRasterizer(&desc, &fillMode[0]);

		desc.FillMode = D3D11_FILL_WIREFRAME;
		States::CreateRasterizer(&desc, &fillMode[1]);
	}

#if CASE
#else
	D3DXMatrixIdentity(&matScale);
	D3DXMatrixScaling(&matScale, scale, scale, scale);

	D3DXMatrixIdentity(&matDist);
	D3DXMatrixTranslation(&matDist, 0, 0, dist);
#endif
}

ExeSphere::~ExeSphere()
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

	for (int i = 0; i < 2; i++)
		SAFE_RELEASE(fillMode[i]);
}

void ExeSphere::Update()
{
#if  CASE

#else
	deltaRadian += rotateSpeed * Time::Delta();

	D3DXMatrixIdentity(&matTrans);
	D3DXMatrixIdentity(&matRot);

	D3DXMatrixTranslation(&matTrans, basePos->x, basePos->y, basePos->z);
	D3DXMatrixRotationY(&matRot, deltaRadian);

	if (isTransduction)
		// 여기서 2번째 matRot은 공전을 위한 회전행렬
		matFinal = matScale * matRot * matDist * matRot * matTrans;
	else
		matFinal = matScale * matRot * matTrans;

	// 공전할 상대를 위해 내 위치 변경
	D3DXVec3TransformCoord(&pos, &D3DXVECTOR3(0, 0, 0), &matFinal);

#endif
}

void ExeSphere::PreRender()
{
}

void ExeSphere::Render()
{
	UINT stride = sizeof(VertexColor); // 그릴 크기
	UINT offset = 0;

	// vertex buffer 여러개 들어갈 수 있음
	// IA 붙는 이유 나중에 설명해주실꺼
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// 그릴 방식 설정
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->RSSetState(fillMode[fillModeNumber]);

	colorBuffer->SetPSBuffer(0); // 레지스터 번호

#if  CASE
#else
	worldBuffer->SetMatrix(matFinal);
#endif

	worldBuffer->SetVSBuffer(1);
	shader->Render();

	// 실제로 그리는 거
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}


void ExeSphere::PostRender()
{
	// ImGui begin 안열면 Debug창에 들어감
	ImGui::SliderInt("Wireframe", &fillModeNumber, 0, 1);
}

void ExeSphere::ResizeScreen()
{
}
