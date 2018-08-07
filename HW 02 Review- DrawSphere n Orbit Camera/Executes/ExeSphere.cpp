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
	, radius(5.0f), stackCount(20), sliceCount(20)
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
			// ���� ���� ������ ó��
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
		// ����
		for (UINT i = 1; i <= sliceCount; i++) {
			indices[index++] = 0;
			indices[index++] = i + 1;
			indices[index++] = i;
		}
		
		// ����
		UINT baseIndex = 1; // �� ������ ������
		UINT ringVertexCount = sliceCount + 1;
		// stackCount - 2 �� ���� ���� 4�� �Ἥ �� ���� �Ʒ����� ���� -1 �������
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

		// �Ʒ���
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
		desc.Usage = D3D11_USAGE_DEFAULT; // ��� ��������� ���� ����
		desc.ByteWidth = sizeof(VertexColor) * vertexCount; // ���� ���ۿ� �� �������� ũ��
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
	// dx�κ��� ��������� release
	// ���� �������� ��������
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
		// ���⼭ 2��° matRot�� ������ ���� ȸ�����
		matFinal = matScale * matRot * matDist * matRot * matTrans;
	else
		matFinal = matScale * matRot * matTrans;

	// ������ ��븦 ���� �� ��ġ ����
	D3DXVec3TransformCoord(&pos, &D3DXVECTOR3(0, 0, 0), &matFinal);

#endif
}

void ExeSphere::PreRender()
{
}

void ExeSphere::Render()
{
	UINT stride = sizeof(VertexColor); // �׸� ũ��
	UINT offset = 0;

	// vertex buffer ������ �� �� ����
	// IA �ٴ� ���� ���߿� �������ֽǲ�
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// �׸� ��� ����
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->RSSetState(fillMode[fillModeNumber]);

	colorBuffer->SetPSBuffer(0); // �������� ��ȣ

#if  CASE
#else
	worldBuffer->SetMatrix(matFinal);
#endif

	worldBuffer->SetVSBuffer(1);
	shader->Render();

	// ������ �׸��� ��
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}


void ExeSphere::PostRender()
{
	// ImGui begin �ȿ��� Debugâ�� ��
	ImGui::SliderInt("Wireframe", &fillModeNumber, 0, 1);
}

void ExeSphere::ResizeScreen()
{
}
