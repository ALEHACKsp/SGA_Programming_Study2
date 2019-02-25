#include "Framework.h"
#include "ScatteringSky.h"

#include "./Viewer/Freedom.h"
#include "./Viewer/Perspective.h"

ScatteringSky::ScatteringSky()
	: realTime(false), theta(D3DX_PI / 2 + D3DX_PI / 4), phi(0), timeSpeed(0.2f)
	, radius(10), slices(32), stacks(16)
{

}

ScatteringSky::~ScatteringSky()
{
	SAFE_DELETE(moonMap);
	SAFE_DELETE(moonglowMap);
	
	SAFE_DELETE(starMap);

	SAFE_DELETE_ARRAY(quadVertices);
	SAFE_RELEASE(quadBuffer);

	SAFE_DELETE(scatterShader);
	SAFE_DELETE(moonShader);
	SAFE_DELETE(cloudsShader);

	SAFE_DELETE(mie2D);
	SAFE_DELETE(rayleigh2D);
	SAFE_DELETE(perlinNoise2D);

	SAFE_DELETE(mieTarget);
	SAFE_DELETE(rayLeighTarget);

	SAFE_RELEASE(perlinNoiseTex);
	SAFE_RELEASE(perlinNoiseSrv);
}

void ScatteringSky::Initialize()
{
	scatterShader = new Shader(Shaders + L"Homework/Scattering.fx");
	moonShader = new Shader(Shaders + L"Homework/Moon.fx");
	cloudsShader = new Shader(Shaders + L"Homework/Clouds.fx");
}

void ScatteringSky::Ready()
{
	mieTarget = new RenderTarget(128, 64);
	rayLeighTarget = new RenderTarget(128, 64);

	GenerateSphere();
	GenerateQuad();
	GeneratePerlinNoiseTex();
	cloudsShader->AsSRV("CloudsMap")->SetResource(perlinNoiseSrv);

	rayleigh2D = new Render2D();
	rayleigh2D->Position(0, 200);
	rayleigh2D->Scale(200, 100);

	mie2D = new Render2D();
	mie2D->Position(0, 100);
	mie2D->Scale(200, 100);

	perlinNoise2D = new Render2D();
	perlinNoise2D->Position(0, 300);
	perlinNoise2D->Scale(100, 100);

	starMap = new Texture(Textures + L"Starfield.png");
	scatterShader->AsSRV("StarMap")->SetResource(starMap->SRV());
	scatterShader->AsSRV("RayleighMap")->SetResource(rayLeighTarget->SRV());
	scatterShader->AsSRV("MieMap")->SetResource(mieTarget->SRV());

	scatterShader->AsSRV("RayleighMap")->SetResource(rayLeighTarget->SRV());
	scatterShader->AsSRV("MieMap")->SetResource(mieTarget->SRV());

	moonMap = new Texture(Textures + L"Moon.png");
	moonShader->AsSRV("MoonMap")->SetResource(moonMap->SRV());
	moonglowMap = new Texture(Textures + L"Moonglow.png");
	moonShader->AsSRV("GlowMap")->SetResource(moonglowMap->SRV());

	// buffer
	{
		cloudCover = -0.1f;
		cloudSharpness = 0.5f;
		numTiles = 16.0f;

		alpha = 1.0f;

		starIntensity = 0.0f;

		waveLength = D3DXVECTOR3(0.65f, 0.57f, 0.475f);
		sampleCount = 20;

		invWaveLength.x = 1.0f / powf(waveLength.x, 4);
		invWaveLength.y = 1.0f / powf(waveLength.y, 4);
		invWaveLength.z = 1.0f / powf(waveLength.z, 4);
		scatterShader->AsVector("InvWaveLength")->SetFloatVector(invWaveLength);

		waveLengthMie.x = powf(waveLength.x, -0.84f);
		waveLengthMie.y = powf(waveLength.y, -0.84f);
		waveLengthMie.z = powf(waveLength.z, -0.84f);
		scatterShader->AsVector("WaveLengthMie")->SetFloatVector(waveLengthMie);
	}

	rayleigh2D->SRV(rayLeighTarget->SRV());
	mie2D->SRV(mieTarget->SRV());
	perlinNoise2D->SRV(perlinNoiseSrv);

	Context::Get()->GetPerspective()->SetFar(10000);
	Context::Get()->UpdateProjection();
}

void ScatteringSky::Update()
{
	if (realTime) {
		//theta = Time::Get()->Running() * D3DX_PI;
		theta += Time::Delta() * timeSpeed;
		//theta += Time::Delta() * 0.1f;
	}

	Context::Get()->GetGlobalLight()->Direction = GetDirection();
	Context::Get()->ChangeGlobalLight();

	// world change
	{
		D3DXMATRIX V;
		D3DXVECTOR3 camPos;
		Context::Get()->GetMainCamera()->Position(&camPos);
		D3DXMatrixTranslation(&V, camPos.x, camPos.y, camPos.z);
		
		scatterShader->AsMatrix("World")->SetMatrix(V);
	}

	UpdateStarIntensity();
}

void ScatteringSky::PreRender()
{
	if (prevTheta == theta && prevPhi == phi) return;

	mieTarget->Set();
	rayLeighTarget->Set();

	ID3D11RenderTargetView* rtvs[2];
	rtvs[0] = rayLeighTarget->RTV();
	rtvs[1] = mieTarget->RTV();
	
	ID3D11DepthStencilView* dsv;
	dsv = rayLeighTarget->DSV();

	D3D::Get()->SetRenderTargets(2, rtvs, dsv);

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	scatterShader->Draw(0, 1, 6);
}

void ScatteringSky::Render()
{
	ImGui::Begin("Sky");
	ImGui::Checkbox("RealTime", &realTime);
	ImGui::DragFloat("Time Speed", &timeSpeed, 0.05f);
	ImGui::DragFloat("Theta", &theta, 0.01f);
	ImGui::DragFloat("Phi", &phi, 0.01f);
	ImGui::End();

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	scatterShader->DrawIndexed(0, 0, indexCount);

	RenderClouds();
	RenderMoon();
	RenderGlow();

	//rayleigh2D->Render();
	//mie2D->Render();
	//perlinNoise2D->Render();
}

void ScatteringSky::GenerateSphere()
{
	UINT domeCount = 32;
	UINT latitude = domeCount / 2; // 위도
	UINT longitude = domeCount; // 경도

	vertexCount = longitude * latitude * 2;
	indexCount = (longitude - 1) * (latitude - 1) * 2 * 6;

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
}

void ScatteringSky::GenerateQuad()
{
	quadVertices = new VertexTexture[6];

	quadVertices[0].Position = D3DXVECTOR3(-1.0f, -1.0f, 0.0f);   //   0
	quadVertices[1].Position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);   //   1
	quadVertices[2].Position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);   //   2
	quadVertices[3].Position = D3DXVECTOR3(1.0f, -1.0f, 0.0f);   //   2
	quadVertices[4].Position = D3DXVECTOR3(-1.0f, 1.0f, 0.0f);   //   1
	quadVertices[5].Position = D3DXVECTOR3(1.0f, 1.0f, 0.0f);   //   3

	quadVertices[0].Uv = D3DXVECTOR2(0, 1);   //   0
	quadVertices[1].Uv = D3DXVECTOR2(0, 0);   //   1
	quadVertices[2].Uv = D3DXVECTOR2(1, 1);   //   2
	quadVertices[3].Uv = D3DXVECTOR2(1, 1);   //   2
	quadVertices[4].Uv = D3DXVECTOR2(0, 0);   //   1
	quadVertices[5].Uv = D3DXVECTOR2(1, 0);   //   3

											  //   CreateVertexBuffer
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

void ScatteringSky::GeneratePerlinNoiseTex()
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

	int gradValues[] = { 1,1,0,
		-1,1,0, 1,-1,0,
		-1,-1,0, 1,0,1,
		-1,0,1, 1,0,-1,
		-1,0,-1, 0,1,1,
		0,-1,1, 0,1,-1,
		0,-1,-1, 1,1,0,
		0,-1,1, -1,1,0,
		0,-1,-1
	};

	//DXGI_FORMAT_R32G32B32A32_FLOAT

	D3D11_TEXTURE2D_DESC textureDesc;
	// Create Texture
	{
		ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
		textureDesc.Width = 256;
		textureDesc.Height = 256;
		// mipmapping은 가까운건 선명하고 뒤에껀 흐려지게 할 때 쓰기 위해 여러장쓰는거
		textureDesc.MipLevels = 1; // mipmapping 안쓸꺼
		textureDesc.ArraySize = 1; // texture안에 여러 장 들어갈 수 있음
								   // 정밀도 높여놓은 담에 렌더링하고 줄일려고 이렇게하는거
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1; // sampling 확대 축소 할 때 간격 맞춰주고 하는거
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		HRESULT hr = D3D::GetDevice()->CreateTexture2D(&textureDesc, NULL, &perlinNoiseTex);
		assert(SUCCEEDED(hr));
	}

	// Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = textureDesc.Format;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(perlinNoiseTex, &desc, &perlinNoiseSrv);
		assert(SUCCEEDED(hr));
	}

	ID3D11Texture2D* srcTexture;
	perlinNoiseSrv->GetResource((ID3D11Resource **)&srcTexture);

	D3D11_TEXTURE2D_DESC srcDesc;
	srcTexture->GetDesc(&srcDesc);

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = srcDesc.Width;
	desc.Height = srcDesc.Height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc = srcDesc.SampleDesc;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_STAGING;

	HRESULT hr;

	ID3D11Texture2D* texture;
	hr = D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture);
	assert(SUCCEEDED(hr));

	hr = D3DX11LoadTextureFromTexture(D3D::GetDC(), srcTexture, NULL, texture);
	assert(SUCCEEDED(hr));

	byte* pixels = new byte[256 * 256 * 4];
	for (int i = 0; i<256; i++)
	{
		for (int j = 0; j<256; j++)
		{
			int offset = (i * 256 + j) * 4;
			byte value = (byte)perm[(j + perm[i]) & 0xFF];
			pixels[offset] = (byte)(gradValues[value & 0x0F] * 64 + 64);
			pixels[offset + 1] = (byte)(gradValues[value & 0x0F + 1] * 64 + 64);
			pixels[offset + 2] = (byte)(gradValues[value & 0x0F + 2] * 64 + 64);
			pixels[offset + 3] = value;
		}
	}

	D3D11_MAPPED_SUBRESOURCE map;
	D3D::GetDC()->Map(texture, 0, D3D11_MAP_WRITE, NULL, &map);
	{
		memcpy(map.pData, pixels, sizeof(byte) * 256 * 256 * 4);
	}
	D3D::GetDC()->Unmap(texture, 0);

	hr = D3DX11LoadTextureFromTexture(D3D::GetDC(), texture, NULL, srcTexture);
	assert(SUCCEEDED(hr));

	SAFE_DELETE_ARRAY(pixels);
	SAFE_RELEASE(srcTexture);
	SAFE_RELEASE(texture);
}

void ScatteringSky::UpdateStarIntensity()
{
	starIntensity = Context::Get()->GetGlobalLight()->Direction.y;
	scatterShader->AsScalar("StarIntensity")->SetFloat(starIntensity);
}

D3DXVECTOR3 ScatteringSky::GetDirection()
{
	float y = cosf(theta);
	float x = sinf(theta) * cosf(phi);
	float z = sin(theta) * sinf(phi);

	return D3DXVECTOR3(x, y, z);
}

// turbidity 흐림
D3DXVECTOR3 ScatteringSky::GetSunColor(float theta, int turbidity)
{
	float beta = 0.04608365822050f * turbidity - 0.04586025928522f;
	float tauR, tauA;
	float tau[3];

	float coseno = cosf(theta + D3DX_PI);
	float factor = theta / D3DX_PI * 180.0f;
	float jarl = powf(93.885f - factor, -1.253f);
	float potencia = jarl;
	float m = 1.0f / (coseno + 0.15f * potencia);

	float lambda[3];
	lambda[0] = waveLength.x;
	lambda[1] = waveLength.y;
	lambda[2] = waveLength.z;

	for (int i = 0; i < 3; i++)
	{
		potencia = powf(lambda[i], 4.0f);
		tauR = expf(-m * 0.008735f * potencia);

		const float alpha = 1.3f;
		potencia = powf(lambda[i], -alpha);
		if (m < 0.0f)
			tau[i] = 0.0f;
		else
		{
			tauA = expf(-m * beta * potencia);
			tau[i] = tauR * tauA;
		}
	}

	D3DXVECTOR3 attenuation = D3DXVECTOR3(tau[0], tau[1], tau[2]);

	//attenuation = D3DXVECTOR3(1, 0, 0);

	return attenuation; // 감쇠
}

void ScatteringSky::RenderGlow()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DXVECTOR3 lightDirection = GetDirection();
	D3DXVECTOR3 camPos;
	Context::Get()->GetMainCamera()->Position(&camPos);

	float pos = 1100;

	D3DXMATRIX world;
	D3DXMATRIX scale, rotationX, rotationY, translation1, translation2;
	D3DXMatrixScaling(&scale, 50, 50, 1);
	D3DXMatrixRotationX(&rotationX, -(theta + D3DX_PI / 2.0f));
	D3DXMatrixRotationY(&rotationY, -(-phi + D3DX_PI / 2.0f));
	D3DXMatrixTranslation(&translation1,
		lightDirection.x * pos, lightDirection.y * pos, lightDirection.z * pos);
	D3DXMatrixTranslation(&translation2, camPos.x, camPos.y, camPos.z);
	world = scale * rotationX * rotationY * translation1 * translation2;
	moonShader->AsMatrix("World")->SetMatrix(world);

	if (theta < D3DX_PI / 2.0f || theta > 3.0f * D3DX_PI / 2.0f)
		alpha = fabs(sinf(theta + D3DX_PI / 2.0f));
	else
		alpha = 0.0f;
	moonShader->AsScalar("Alpha")->SetFloat(alpha);

	moonShader->Draw(0, 1, 6);
}

void ScatteringSky::RenderMoon()
{
	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DXVECTOR3 lightDirection = GetDirection();
	D3DXVECTOR3 camPos;
	Context::Get()->GetMainCamera()->Position(&camPos);

	float pos = 2200;

	D3DXMATRIX world;
	D3DXMATRIX scale, rotationX, rotationY, translation1, translation2;
	D3DXMatrixScaling(&scale, 50, 50, 1);
	D3DXMatrixRotationX(&rotationX, -(theta + D3DX_PI / 2.0f));
	D3DXMatrixRotationY(&rotationY, -(-phi + D3DX_PI / 2.0f));
	D3DXMatrixTranslation(&translation1,
		lightDirection.x * pos, lightDirection.y * pos, lightDirection.z * pos);
	D3DXMatrixTranslation(&translation2, camPos.x, camPos.y, camPos.z);
	world = scale * rotationX * rotationY * translation1 * translation2;
	moonShader->AsMatrix("World")->SetMatrix(world);

	//if (theta < D3DX_PI / 2.0f || theta > 3.0f * D3DX_PI / 2.0f)
	//	alpha = fabs(sinf(theta + D3DX_PI / 2.0f));
	//else
	//	alpha = 0.0f;
	//moonShader->AsScalar("Alpha")->SetFloat(alpha);

	moonShader->Draw(0, 0, 6);
}

void ScatteringSky::RenderClouds()
{
	float inverseCloudVelocity = 16.0f;

	UINT stride = sizeof(VertexTexture);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &quadBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3DXVECTOR3 camPos;
	Context::Get()->GetMainCamera()->Position(&camPos);

	D3DXMATRIX world;
	D3DXMATRIX scale, rotationX, rotationY, translation1, translation2;
	D3DXMatrixScaling(&scale, 10000.0f, 10000.0f, 1.0f);
	D3DXMatrixTranslation(&translation1, 0, 0, 2201);
	D3DXMatrixRotationX(&rotationX, -D3DX_PI / 2.0f);
	D3DXMatrixTranslation(&translation2, camPos.x, camPos.y, camPos.z);
	world = scale * translation1 * rotationX * translation2;
	
	//D3DXMatrixIdentity(&world);
	cloudsShader->AsMatrix("World")->SetMatrix(world);

	time = Time::Get()->Running() / inverseCloudVelocity;
	cloudsShader->AsScalar("CloudTime")->SetFloat(time);
	sunColor = GetSunColor(-theta, 2);
	cloudsShader->AsVector("SunColor")->SetFloatVector(sunColor);

	cloudsShader->Draw(0, 0, 6);
}
