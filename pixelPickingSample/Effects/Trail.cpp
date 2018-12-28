#include "stdafx.h"
#include "Trail.h"

Trail::Trail(D3DXMATRIX* transform, D3DXVECTOR3& translation, D3DXVECTOR3& rotationDegree, float length, UINT trailNum, float time)
	: world(transform), translation(translation), rotation(rotationDegree), scale(1, 1, 1)
	, length(length), curTime(0), time(time), head(0)
{
	assert(trailNum < 128);

	trailBuffer = new TrailBuffer();
	trailBuffer->Data.TrailNum = trailNum;
	trailBuffer->Data.TrailNumInv = 1.0f / (float)(trailNum);
	trailBuffer->Data.Color = D3DXCOLOR(1, 0, 0, 1);

	UpdateTransform();

	//매트릭스 저장해둘곳
	circularMatrix = new D3DXMATRIX[trailNum];
	for (UINT i = 0; i < trailNum; i++)
		circularMatrix[i] = this->transform * (*world);

	GenerateTrail();
	
	texture = new Texture(Textures + L"White.png");
	shader = new Shader(Shaders + L"046_Trail.hlsl");

	cullMode[0] = new RasterizerState();
	cullMode[1] = new RasterizerState();
	cullMode[1]->CullMode(D3D11_CULL_NONE);

	blendMode[0] = new BlendState();
	blendMode[1] = new BlendState();
	blendMode[1]->BlendEnable(true);
	blendMode[1]->DestBlend(D3D11_BLEND_ONE);
	blendMode[1]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	blendMode[1]->BlendOp(D3D11_BLEND_OP_ADD);

	depthMode[0] = new DepthStencilState();
	depthMode[1] = new DepthStencilState();
	depthMode[1]->DepthEnable(false);
}

Trail::~Trail()
{
	SAFE_DELETE(cullMode[0]);
	SAFE_DELETE(cullMode[1]);

	SAFE_DELETE(blendMode[0]);
	SAFE_DELETE(blendMode[1]);

	SAFE_DELETE(depthMode[0]);
	SAFE_DELETE(depthMode[1]);

	SAFE_DELETE_ARRAY(circularMatrix);

	SAFE_DELETE(vertices);
	SAFE_DELETE(indices);

	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE(shader);
	SAFE_DELETE(texture);

	SAFE_DELETE(trailBuffer);
}

void Trail::Update()
{
	UpdateTransform();
	Move();
	UpdateTrailBuffer();
}

void Trail::Render()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	trailBuffer->SetVSBuffer(10);
	texture->SetShaderResource(10);
	
	depthMode[1]->OMSetDepthStencilState();
	blendMode[1]->OMSetBlendState();
	cullMode[1]->RSSetState();
	shader->Render();
	D3D::GetDC()->Draw(6 * trailBuffer->Data.TrailNum, 0);
	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	cullMode[0]->RSSetState();
	blendMode[0]->OMSetBlendState();
	depthMode[0]->OMSetDepthStencilState();

	RenderImGui();
}

void Trail::RenderImGui()
{
	ImGui::Begin("Trail");
	ImGui::SliderFloat3("Translation", (float*)&translation, -30.0f, 30.0f);
	ImGui::SliderFloat3("Rotation", (float*)&rotation, -180.0f, 180.0f);
	ImGui::End();
}

void Trail::GenerateTrail()
{
	vertexCount = 2 * (trailBuffer->Data.TrailNum + 1);
	vertices = new VertexTexture[vertexCount];

	//Create VertexData
	{
		for (UINT y = 0; y < 2; y++)
		{
			for (UINT x = 0; x < trailBuffer->Data.TrailNum + 1; x++)
			{
				UINT index = (trailBuffer->Data.TrailNum + 1) * y + x;

				vertices[index].Position.x = 0;
				vertices[index].Position.y = (float)length * (float)y;
				vertices[index].Position.z = 0;

				vertices[index].Uv.x = (float)x / (float)trailBuffer->Data.TrailNum;
				vertices[index].Uv.y = (float)y;
			}
		}//for(y)
	}

	indexCount = trailBuffer->Data.TrailNum * 6;
	indices = new UINT[indexCount];

	//CreateIndexData
	{
		UINT index = 0;
		for (UINT y = 0; y < 2; y++)
		{
			for (UINT x = 0; x < trailBuffer->Data.TrailNum + 1; x++)
			{
				indices[index + 0] = (trailBuffer->Data.TrailNum + 1) * y + x; //0
				indices[index + 1] = (trailBuffer->Data.TrailNum + 1) * (y + 1) + x; //2
				indices[index + 2] = (trailBuffer->Data.TrailNum + 1) * y + x + 1; //1

				indices[index + 3] = (trailBuffer->Data.TrailNum + 1) * y + x + 1; //1
				indices[index + 4] = (trailBuffer->Data.TrailNum + 1) * (y + 1) + x; //2
				indices[index + 5] = (trailBuffer->Data.TrailNum + 1) * (y + 1) + x + 1; //3

				index += 6;
			}
		}//for(y)
	}

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//CreateIndexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = indices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
}

void Trail::SetVertexBuffer()
{
}

void Trail::Move()
{
	////시간 검사
	//curTime += Time::Get()->Delta();
	//if (curTime < time)
	//{
	//	//시간이 안 지났으니 맨앞에놈만 월드 변경해줌
	//	circularMatrix[head] = transform * (*world);
	//	
	//	return;
	//}
	//
	//curTime -= time;

	//시간 지났으면 지난 매트릭스 정보 업데이트 해줌
	//circular
	circularMatrix[head] = transform * (*world);
	head = (head >= trailBuffer->Data.TrailNum) ? head = 0 : ++head;

	//savePosition
}

void Trail::UpdateTrailBuffer()
{
	UINT index = head;
	for (UINT i = 0; i < trailBuffer->Data.TrailNum; i++)
	{
		D3DXMatrixTranspose(&trailBuffer->Data.Worlds[i], &circularMatrix[index]);
		index = (index == 0) ? trailBuffer->Data.TrailNum - 1 : index - 1;
	}
}

void Trail::UpdateTransform()
{
	D3DXMATRIX S, T, R;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixTranslation(&T, translation.x, translation.y, translation.z);
	D3DXMatrixRotationYawPitchRoll(&R, Math::ToRadian(rotation.y), Math::ToRadian(rotation.x), Math::ToRadian(rotation.z));
	transform = S * T * R;
}
