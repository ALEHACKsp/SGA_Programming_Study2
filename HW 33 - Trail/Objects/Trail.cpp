#include "stdafx.h"
#include "Trail.h"

#include "GameModel.h"

Trail::Trail(int count)
	:count(count), deltaTime(0), changeTime(0.0001f)
	, bBone(false), bCull(false), bDepth(false), bRender(false)
{
	worldBuffer = new WorldBuffer();
	vsBuffer = new VsBuffer();
	vsBuffer->Data.Count = count;

	psBuffer = new PsBuffer();

	texture = new Texture(Textures + L"RedTrail.png");

	// shader
	shader = new Shader(Shaders + L"Homework/Trail.hlsl");

	// Create Vertex Buffer
	{
		VertexTexture* vertices = new VertexTexture[6 * count];
		
		float dx = 1.0f / count; // 비율

		for (UINT i = 0; i < count; i++)
		{
			vertices[i * 6 + 0].Position = D3DXVECTOR3(-0.5f + (dx * i), -0.5f, 0.0f);
			vertices[i * 6 + 1].Position = D3DXVECTOR3(-0.5f + (dx * i), 0.5f, 0.0f);
			vertices[i * 6 + 2].Position = D3DXVECTOR3(-0.5f + dx * (i + 1), -0.5f, 0.0f);
			vertices[i * 6 + 3].Position = D3DXVECTOR3(-0.5f + dx * (i + 1), -0.5f, 0.0f);
			vertices[i * 6 + 4].Position = D3DXVECTOR3(-0.5f + (dx * i), 0.5f, 0.0f);
			vertices[i * 6 + 5].Position = D3DXVECTOR3(-0.5f + dx * (i + 1), 0.5f, 0.0f);

			vertices[i * 6 + 0].Uv = D3DXVECTOR2((dx * i), 1);
			vertices[i * 6 + 1].Uv = D3DXVECTOR2((dx * i), 0);
			vertices[i * 6 + 2].Uv = D3DXVECTOR2(dx * (i + 1), 1);
			vertices[i * 6 + 3].Uv = D3DXVECTOR2(dx * (i + 1), 1);
			vertices[i * 6 + 4].Uv = D3DXVECTOR2((dx * i), 0);
			vertices[i * 6 + 5].Uv = D3DXVECTOR2(dx * (i + 1), 0);
		}

		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * 6 * count;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));

		SAFE_DELETE_ARRAY(vertices);
	}

	cullMode[0] = new RasterizerState();
	cullMode[1] = new RasterizerState();
	cullMode[1]->CullMode(D3D11_CULL_NONE);

	depthMode[0] = new DepthStencilState();
	depthMode[1] = new DepthStencilState();
	depthMode[1]->DepthEnable(false);

	blendMode[0] = new BlendState();
	blendMode[1] = new BlendState();
	blendMode[1]->BlendEnable(true);
	blendMode[1]->DestBlend(D3D11_BLEND_ONE);
	blendMode[1]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	blendMode[1]->BlendOp(D3D11_BLEND_OP_ADD);

	D3DXMatrixIdentity(&matrix);

	model = NULL;

	//Scale(10.0f, 10.0f, 1.0f);
	//RotationDegree(90, -90, 0);
}

Trail::~Trail()
{
	SAFE_DELETE(depthMode[0]);
	SAFE_DELETE(depthMode[1]);

	SAFE_DELETE(cullMode[0]);
	SAFE_DELETE(cullMode[1]);

	SAFE_DELETE(blendMode[0]);
	SAFE_DELETE(blendMode[1]);

	SAFE_RELEASE(vertexBuffer);
	SAFE_DELETE(texture);
	SAFE_DELETE(shader);
	SAFE_DELETE(vsBuffer);
	SAFE_DELETE(psBuffer);
	SAFE_DELETE(worldBuffer);
}

void Trail::Update()
{
	deltaTime += Time::Delta();

	// test
	//Position(model->BonePosition(model->GetModel()->BoneByName(boneName)->Index()));

	if (deltaTime >= changeTime) {
		deltaTime = 0;
		ShiftMatrix();
	}
}

void Trail::Render()
{
	worldBuffer->SetMatrix(World());
	worldBuffer->SetVSBuffer(1);

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	vsBuffer->SetVSBuffer(5);
	psBuffer->SetPSBuffer(5);
	texture->SetShaderResource(5);
	
	blendMode[1]->OMSetBlendState();
	if(bDepth == false)
	depthMode[1]->OMSetDepthStencilState();
	if(bCull == false)
	cullMode[1]->RSSetState();
	shader->Render();
	D3D::GetDC()->Draw(6 * count, 0);
	cullMode[0]->RSSetState();
	depthMode[0]->OMSetDepthStencilState();
	blendMode[0]->OMSetBlendState();
}

void Trail::PostRender()
{
	ImGui::Begin("Trail");

	D3DXVECTOR3 position = Position();
	D3DXVECTOR3 rotation = RotationDegree();
	D3DXVECTOR3 scale = Scale();
	if (ImGui::DragFloat3("Position", (float*)&position, 0.1f)) Position(position);
	if (ImGui::DragFloat3("Rotation", (float*)&rotation, 0.1f)) RotationDegree(rotation);
	if (ImGui::DragFloat3("Scale", (float*)&scale, 0.1f)) Scale(scale);
	ImGui::DragFloat("ChangeTime", &changeTime, 0.1f, 0,0,"%.5f");
	ImGui::Checkbox("CullMode", &bCull);
	ImGui::Checkbox("DepthMode", &bDepth);
	ImGui::Checkbox("UseTexture", (bool*)&psBuffer->Data.Select);
	ImGui::ColorEdit4("Color", (float*) &psBuffer->Data.Color);

	ImGui::End();
}

void Trail::SetRender(bool bRender) { 
	this->bRender = bRender; 
}

void Trail::ShiftMatrix()
{
	// 첫 위치 조정
	if (bRender == false) {
		bRender = true;

		if (bBone) {
			matrix = model->GetBoneMatrix(boneName);
			matrix = World() * matrix;
		}
		else {
			matrix = model->World();
			matrix = World() * matrix;
		}
		D3DXMatrixTranspose(&matrix, &matrix);

		for (int i = 0; i < count; i++) {
			vsBuffer->Data.Worlds[i] = matrix;
		}

		return;
	}

	for (int i = count-1; i > 0; i--) {
		vsBuffer->Data.Worlds[i] = vsBuffer->Data.Worlds[i - 1];
	}

	//memcpy 같은 방법 더 빠를듯
	//(
	//	&buffer->Data.worlds[1],
	//	&buffer->Data.worlds[0],
	//	sizeof(D3DXMATRIX) * (count - 1)
	//);

	if (bBone) {
		matrix = model->GetBoneMatrix(boneName);
		matrix = World() * matrix;
	}
	else {
		matrix = model->World();
		matrix = World() * matrix;
	}

	D3DXMatrixTranspose(&matrix, &matrix);
	vsBuffer->Data.Worlds[0] = matrix;
}
