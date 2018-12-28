#include "stdafx.h"
#include "Sky.h"
#include "../Objects/GameModel.h"

Sky::Sky(ExecuteValues* value)
	: values(value), realTime(true), theta(0), phi(0), domeCount(32), permTex(NULL)
	//, radius (10), slices (32), stacks (16)
{
	mieTarget = new RenderTarget(values, 128, 64);
	rayleighTarget = new RenderTarget(values, 128, 64);

	scatterShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Scattering", "PS_Scattering");
	targetShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Target", "PS_Target");
	moonShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Moon", "PS_Moon");
	cloudShader = new Shader(Shaders + L"044_Scattering.hlsl", "VS_Cloud", "PS_Cloud");

	worldBuffer = new WorldBuffer();
	targetBuffer = new TargetBuffer();
	scatterBuffer = new ScatterBuffer();
	moonBuffer = new MoonBuffer();
	cloudBuffer = new CloudBuffer();
	
	GenerateSphere();
	GenerateQuad();
	GeneratePermTex();

	depthStencilState[0] = new DepthStencilState();
	depthStencilState[1] = new DepthStencilState();
	depthStencilState[1]->DepthEnable(false);
	depthStencilState[1]->DepthEnable(D3D11_DEPTH_WRITE_MASK_ZERO);

	rayleigh2D = new Render2D(values);
	rayleigh2D->Position(0, 100);
	rayleigh2D->Scale(200, 100);
	mie2D = new Render2D(values);
	mie2D->Position(0, 0);
	mie2D->Scale(200, 100);

	starField = new Texture(Textures + L"Starfield.png");
	glowTex = new Texture(Textures + L"MoonGlow.png");
	moonTex = new Texture(Textures + L"Moon.png");

	skySampler = new SamplerState();
	skySampler->AddressU(D3D11_TEXTURE_ADDRESS_CLAMP);
	skySampler->AddressV(D3D11_TEXTURE_ADDRESS_CLAMP);
	skySampler->AddressW(D3D11_TEXTURE_ADDRESS_CLAMP);
	skySampler->Filter(D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	quadSampler = new SamplerState();
	quadSampler->AddressU(D3D11_TEXTURE_ADDRESS_CLAMP);
	quadSampler->AddressV(D3D11_TEXTURE_ADDRESS_CLAMP);
	quadSampler->AddressW(D3D11_TEXTURE_ADDRESS_CLAMP);
	quadSampler->Filter(D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	blendState[0] = new BlendState();
	blendState[1] = new BlendState();
	blendState[1]->BlendEnable(true);
}

Sky::~Sky()
{
	SAFE_DELETE(depthStencilState[0]);
	SAFE_DELETE(depthStencilState[1]);
	SAFE_DELETE(skySampler);
	SAFE_DELETE(quadSampler);
	
	SAFE_DELETE(starField);
	SAFE_DELETE(glowTex);
	SAFE_DELETE(moonTex);
	SAFE_DELETE(permTex);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(targetBuffer);
	SAFE_DELETE(scatterBuffer);
	SAFE_DELETE(moonBuffer);
	SAFE_DELETE(cloudBuffer);

	SAFE_DELETE_ARRAY(quadVertices);
	SAFE_RELEASE(quadBuffer);
	SAFE_RELEASE(vertexBuffer);
	SAFE_RELEASE(indexBuffer);

	SAFE_DELETE(scatterShader);
	SAFE_DELETE(targetShader);

	SAFE_DELETE(mieTarget);
	SAFE_DELETE(rayleighTarget);

	SAFE_DELETE(rayleigh2D);
	SAFE_DELETE(mie2D);
}

void Sky::Update()
{
	D3DXMATRIX V;
	D3DXVECTOR3 position;
	values->MainCamera->Position(&position);
	D3DXMatrixTranslation(&V, position.x, position.y, position.z);

	worldBuffer->SetMatrix(V);

	if (realTime)
	{
		int minutos = (int) (Time::Get()->Running() * 60.0f);
		theta = (float)minutos * (float)(Math::PI) / 12.0f / 60.0f;
	}

	values->GlobalLight->Data.Direction = GetDirection();
}

void Sky::PreRender()
{
	if (prevTheta == theta && prevPhi == phi)
		return;

	prevTheta = theta;
	prevPhi = phi;

	values->GlobalLight->Data.Color = GetSunColor(-theta, 2);

	//multi render
	{
		mieTarget->Set();
		rayleighTarget->Set();

		ID3D11RenderTargetView* rtvs[2];
		rtvs[0] = rayleighTarget->GetRTV();
		rtvs[1] = mieTarget->GetRTV();
		ID3D11DepthStencilView* dsv;
		dsv = rayleighTarget->GetDSV();
		D3D::Get()->SetRenderTargets(2, rtvs, dsv);
	}

	//draw
	{
		UINT stride = sizeof(VertexTexture);
		UINT offset = 0;

		D3D::GetDC()->IASetVertexBuffers(0, 1, &quadBuffer, &stride, &offset);
		D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		targetBuffer->SetPSBuffer(11);
		targetShader->Render();

		D3D::GetDC()->Draw(6, 0);
	}
}

void Sky::Render()
{
	values->Perspective->SetZf(10000.0f);
	depthStencilState[1]->OMSetDepthStencilState();
	DrawScatter();
	DrawGlow();
	DrawMoon();
	DrawCloud();
	depthStencilState[0]->OMSetDepthStencilState();
	//values->Perspective->SetZf();


	//산란효과 표시
	rayleigh2D->SRV(rayleighTarget->GetSRV());
	rayleigh2D->Update();
	rayleigh2D->Render();

	mie2D->SRV(mieTarget->GetSRV());
	mie2D->Update();
	mie2D->Render();
}

void Sky::GenerateSphere()
{
	UINT latitude = domeCount / 2; // 위도
	UINT longitude = domeCount; // 경도

	vertexCount = longitude * latitude * 2;
	indexCount = (longitude - 1) * (latitude - 1) * 2 * 8;

	VertexTexture* vertices = new VertexTexture[vertexCount];

	UINT index = 0;
	for (UINT i = 0; i < longitude; i++)
	{
		float xz = 100.0f * (i / (longitude - 1.0f)) * Math::PI / 180.0f;

		for (UINT j = 0; j < latitude; j++)
		{
			float y = Math::PI * j / (latitude - 1);

			vertices[index].Position.x = sinf(xz) * cosf(y);
			vertices[index].Position.y = cosf(xz);
			vertices[index].Position.z = sinf(xz) * sinf(y);
			vertices[index].Position *= 10.0f; // 크기를 키우려고 임의의 값 곱한거

			vertices[index].Uv.x = 0.5f / (float)longitude + i / (float)longitude;
			vertices[index].Uv.y = 0.5f / (float)latitude + j / (float)latitude;

			index++;
		} // for(j)
	}  // for(i)

	for (UINT i = 0; i < longitude; i++)
	{
		float xz = 100.0f * (i / (longitude - 1.0f)) * Math::PI / 180.0f;

		for (UINT j = 0; j < latitude; j++)
		{
			float y = (Math::PI * 2.0f) - (Math::PI * j / (latitude - 1));

			vertices[index].Position.x = sinf(xz) * cosf(y);
			vertices[index].Position.y = cosf(xz);
			vertices[index].Position.z = sinf(xz) * sinf(y);
			vertices[index].Position *= 10.0f; // 크기를 키우려고 임의의 값 곱한거

			vertices[index].Uv.x = 0.5f / (float)longitude + i / (float)longitude;
			vertices[index].Uv.y = 0.5f / (float)latitude + j / (float)latitude;

			index++;
		} // for(j)
	}  // for(i)


	index = 0;
	UINT* indices = new UINT[indexCount * 3];

	for (UINT i = 0; i < longitude - 1; i++)
	{
		for (UINT j = 0; j < latitude - 1; j++)
		{
			indices[index++] = i * latitude + j;
			indices[index++] = (i + 1) * latitude + j;
			indices[index++] = (i + 1) * latitude + (j + 1);

			indices[index++] = (i + 1) * latitude + (j + 1);
			indices[index++] = i * latitude + (j + 1);
			indices[index++] = i * latitude + j;
		}
	}

	UINT offset = latitude * longitude;
	for (UINT i = 0; i < longitude - 1; i++)
	{
		for (UINT j = 0; j < latitude - 1; j++)
		{
			indices[index++] = offset + i * latitude + j;
			indices[index++] = offset + (i + 1) * latitude + (j + 1);
			indices[index++] = offset + (i + 1) * latitude + j;

			indices[index++] = offset + i * latitude + (j + 1);
			indices[index++] = offset + (i + 1) * latitude + (j + 1);
			indices[index++] = offset + i * latitude + j;
		}
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

	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	//float sliceStep = (Math::PI * 2) / slices;
	//float stackStep = (Math::PI) / (stacks);
	//
	/////Create Vertex Data 
	//vertexCount = (slices + 1) * (stacks + 1);
	//VertexTexture* vertices = new VertexTexture[vertexCount];
	//{
	//	float stackAngle = 0.0f; // - (Math::PI / 2);
	//	for (UINT y = 0; y <= stacks; y++) // 위도, altitude
	//	{
	//		float sliceAngle = 0.0f;
	//		for (UINT xz = 0; xz <= slices; xz++) // 경도, longitude
	//		{
	//			UINT index = (slices + 1) * y + xz;
	//
	//			vertices[index].Position.x = (float)(radius * cosf(stackAngle) * cosf(sliceAngle));
	//			vertices[index].Position.y = (float)(radius * sinf(stackAngle));
	//			vertices[index].Position.z = (float)(radius * cosf(stackAngle) * sinf(sliceAngle));
	//
	//			vertices[index].Uv.x = (1 / (float)slices) * xz;
	//			vertices[index].Uv.y = (1 / (float)stacks) * y;
	//
	//			sliceAngle += sliceStep;
	//		}
	//		stackAngle += stackStep;
	//	}
	//}
	//
	/////Create Index Data 
	//indexCount = slices * stacks * 6;
	//UINT* indices = new UINT[indexCount];
	//{
	//	UINT index = 0;
	//	for (UINT y = 0; y < stacks; y++)
	//	{
	//		for (UINT x = 0; x < slices; x++)
	//		{
	//			indices[index + 0] = (slices + 1) * y + x; //0
	//			indices[index + 1] = (slices + 1) * (y + 1) + x; //1
	//			indices[index + 2] = (slices + 1) * y + (x + 1); //2
	//
	//			indices[index + 3] = (slices + 1) * y + (x + 1); //2
	//			indices[index + 4] = (slices + 1) * (y + 1) + x; //1
	//			indices[index + 5] = (slices + 1) * (y + 1) + (x + 1); //3
	//
	//			index += 6;
	//		}
	//	}//for(z)
	//}
	//
	////CreateVertexBuffer
	//{
	//	D3D11_BUFFER_DESC desc = { 0 };
	//	desc.Usage = D3D11_USAGE_DEFAULT;
	//	desc.ByteWidth = sizeof(VertexTexture) * vertexCount;
	//	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//
	//	D3D11_SUBRESOURCE_DATA data = { 0 };
	//	data.pSysMem = vertices;
	//
	//	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
	//	assert(SUCCEEDED(hr));
	//}
	//
	////CreateIndexBuffer
	//{
	//	D3D11_BUFFER_DESC desc = { 0 };
	//	desc.Usage = D3D11_USAGE_DEFAULT;
	//	desc.ByteWidth = sizeof(UINT) * indexCount;
	//	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//
	//	D3D11_SUBRESOURCE_DATA data = { 0 };
	//	data.pSysMem = indices;
	//
	//	HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
	//	assert(SUCCEEDED(hr));
	//}
	//
	//SAFE_DELETE_ARRAY(vertices);
	//SAFE_DELETE_ARRAY(indices);
}

void Sky::GenerateQuad()
{
	quadVertices = new VertexTexture[6];
	quadVertices[0].Position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);
	quadVertices[1].Position = D3DXVECTOR3(-1.0f,  1.0f, 0.0f);
	quadVertices[2].Position = D3DXVECTOR3( 1.0f, -1.0f, 0.0f);
	quadVertices[3].Position = D3DXVECTOR3( 1.0f, -1.0f, 0.0f);
	quadVertices[4].Position = D3DXVECTOR3(-1.0f,  1.0f, 0.0f);
	quadVertices[5].Position = D3DXVECTOR3( 1.0f,  1.0f, 0.0f);
	quadVertices[0].Uv = D3DXVECTOR2(0, 1);
	quadVertices[1].Uv = D3DXVECTOR2(0, 0);
	quadVertices[2].Uv = D3DXVECTOR2(1, 1);
	quadVertices[3].Uv = D3DXVECTOR2(1, 1);
	quadVertices[4].Uv = D3DXVECTOR2(0, 0);
	quadVertices[5].Uv = D3DXVECTOR2(1, 0);

	//CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexTexture) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = quadVertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &quadBuffer);
		assert(SUCCEEDED(hr));
	}
}

void Sky::GeneratePermTex()
{
	int perm[] = { 151,160,137,91,90,15,
		131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
		190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
		88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
		77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
		102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
		135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
		5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
		223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
		129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
		251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
		49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};

	int gradValues[] = {
		1,1,0,
		-1,1,0, 1,-1,0,
		-1,-1,0, 1,0,1,
		-1,0,1, 1,0,-1,
		-1,0,-1, 0,1,1,
		0,-1,1, 0,1,-1,
		0,-1,-1, 1,1,0,
		0,-1,1, -1,1,0,
		0,-1,-1
	};

	permTex = new Texture(Textures + L"PermTex.png");

	vector<D3DXCOLOR> pixels;
	for (int i = 0; i<256; i++)
	{
		for (int j = 0; j<256; j++)
		{
			int value = perm[(j + perm[i]) & 0xFF];
			D3DXCOLOR color;
			color.r = (float)(gradValues[value & 0x0F] * 64 + 64);
			color.g = (float)(gradValues[value & 0x0F + 1] * 64 + 64);
			color.b = (float)(gradValues[value & 0x0F + 2] * 64 + 64);
			color.a = (float)value;
			pixels.push_back(color);
		}
	}
	permTex->WritePixels(DXGI_FORMAT_R8G8B8A8_UNORM, pixels, true);
}

D3DXVECTOR3 Sky::GetDirection()
{
	float y = cosf(theta);
	float x = sinf(theta) * cosf(phi);
	float z = sinf(theta) * sinf(phi);

	return D3DXVECTOR3(x, y, z);
}

D3DXCOLOR Sky::GetSunColor(float theta, int turbidity)
{
	float beta = 0.04608365822050f * turbidity - 0.04586025928522f;
	float tauR, tauA;
	float tau[3];

	float coseno = cosf(theta + Math::PI);
	double factor = (double)theta / Math::PI * 180.0;
	double jarl = pow(93.885 - factor, -1.253);
	float potencia = (float)jarl;
	float m = 1.0f / (coseno + 0.15f * potencia);

	int i;
	float lambda[3];
	lambda[0] = targetBuffer->Data.WaveLength.x;
	lambda[1] = targetBuffer->Data.WaveLength.y;
	lambda[2] = targetBuffer->Data.WaveLength.z;


	for (i = 0; i < 3; i++)
	{
		potencia = pow(lambda[i], 4.0f);
		tauR = exp(-m * 0.008735f * potencia);

		const float alpha = 1.3f;
		potencia = pow(lambda[i], -alpha);
		if (m < 0.0f)
			tau[i] = 0.0f;
		else
		{
			tauA = exp(-m * beta * potencia);
			tau[i] = tauR * tauA;
		}

	}

	D3DXCOLOR vAttenuation = D3DXCOLOR(tau[0], tau[1], tau[2], 1.0f);
	return vAttenuation;
}

void Sky::DrawScatter()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	scatterShader->Render();
	worldBuffer->SetVSBuffer(1);

	ID3D11ShaderResourceView* srv;
	srv = rayleighTarget->GetSRV();
	D3D::GetDC()->PSSetShaderResources(10, 1, &srv);
	srv = mieTarget->GetSRV();
	D3D::GetDC()->PSSetShaderResources(11, 1, &srv);

	skySampler->PSSetSamplers(10);

	starField->SetShaderResource(12);

	scatterBuffer->Data.StarIntensity = values->GlobalLight->Data.Direction.y;
	scatterBuffer->SetPSBuffer(10);

	D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
}

void Sky::DrawGlow()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DXMATRIX world;
	D3DXVECTOR3 position;
	position = values->MainCamera->Position();
	D3DXMatrixRotationYawPitchRoll(&world, -phi + Math::PI / 2.0f, theta + Math::PI / 2.0f + Math::PI, 0.0f);
	world._41 = values->GlobalLight->Data.Direction.x * 5 + position.x;
	world._42 = values->GlobalLight->Data.Direction.y * 5 + position.y;
	world._43 = values->GlobalLight->Data.Direction.z * 5 + position.z;
	
	moonShader->Render();

	worldBuffer->SetMatrix(world);
	worldBuffer->SetVSBuffer(1);

	if (theta < Math::PI / 2.0f || theta > 3.0f * Math::PI / 2.0f)
		moonBuffer->Data.Alpha = abs(sinf(theta + Math::PI / 2.0f));
	else
		moonBuffer->Data.Alpha = 0.0f;

	moonBuffer->SetPSBuffer(10);
	glowTex->SetShaderResource(10);

	blendState[1]->OMSetBlendState();
	D3D::GetDC()->Draw(6, 0);
	blendState[0]->OMSetBlendState();
}

void Sky::DrawMoon()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DXMATRIX world;
	D3DXVECTOR3 position;
	position = values->MainCamera->Position();
	D3DXMatrixRotationYawPitchRoll(&world, -phi + Math::PI / 2.0f, theta + Math::PI / 2.0f + Math::PI, 0.0f);
	world._41 = values->GlobalLight->Data.Direction.x * 15.0f + position.x;
	world._42 = values->GlobalLight->Data.Direction.y * 15.0f + position.y;
	world._43 = values->GlobalLight->Data.Direction.z * 15.0f + position.z;

	moonShader->Render();

	worldBuffer->SetMatrix(world);
	worldBuffer->SetVSBuffer(1);

	if (theta < Math::PI / 2.0f || theta > 3.0f * Math::PI / 2.0f)
		moonBuffer->Data.Alpha = abs(sinf(theta + Math::PI / 2.0f));
	else
		moonBuffer->Data.Alpha = 0.0f;

	moonBuffer->SetPSBuffer(10);
	moonTex->SetShaderResource(10);
	quadSampler->PSSetSamplers(10);

	blendState[1]->OMSetBlendState();
	D3D::GetDC()->Draw(6, 0);
	blendState[0]->OMSetBlendState();
}

void Sky::DrawCloud()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DXMATRIX S, T, R, T2;
	D3DXVECTOR3 position;
	position = values->MainCamera->Position();
	D3DXMatrixScaling(&S, 900.0f, 900.0f, 1.0f);
	D3DXMatrixTranslation(&T, 0, 0, 200.0f);
	D3DXMatrixRotationX(&R, Math::PI * 1.5f);
	D3DXMatrixTranslation(&T2, position.x, 0.0f, position.z);

	cloudShader->Render();
	permTex->SetShaderResource(10);
	quadSampler->PSSetSamplers(10);

	worldBuffer->SetMatrix(S * T * R * T2);
	worldBuffer->SetVSBuffer(1);

	//구름속도
	cloudBuffer->Data.Time = Time::Get()->Running() / 16.0f;
	cloudBuffer->SetPSBuffer(10);

	blendState[1]->OMSetBlendState();
	D3D::GetDC()->Draw(6, 0);
	blendState[0]->OMSetBlendState();
}
