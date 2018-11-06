#include "stdafx.h"
#include "InstancedCloudManager.h"

#include "ParticleInstancer.h"

InstancedCloudManager::InstancedCloudManager(ExecuteValues* values)
	:values(values)
{
	clouds = new ParticleInstancer();
	
	blendState[0] = new BlendState();
	blendState[1] = new BlendState();
	// NonePremultiplied
	/*
		Blend.SourceAlpha			- D3D11_BLEND_SRC_ALPHA			|| colorSourceBlend	- SrcBlend
		Blend.SourceAlpha			- D3D11_BLEND_SRC_ALPHA			|| alphaSourceBlend	- SrcBlendAlpha
		Blend.InverseSourceAlpha	- D3D11_BLEND_INV_SRC_ALPHA		|| colorDestBlend	- DestBlend
		Blend.InverseSourceAlpha	- D3D11_BLEND_INV_SRC_ALPHA		|| alphaDestBlend	- DestBlendAlpha
	*/
	blendState[1]->BlendEnable(true);
	blendState[1]->SrcBlend(D3D11_BLEND_SRC_ALPHA);
	blendState[1]->SrcBlendAlpha(D3D11_BLEND_SRC_ALPHA);
	blendState[1]->DestBlend(D3D11_BLEND_INV_SRC_ALPHA);
	blendState[1]->DestBlendAlpha(D3D11_BLEND_INV_SRC_ALPHA);

	rasterizerState[0] = new RasterizerState();
	rasterizerState[1] = new RasterizerState();
	rasterizerState[1]->CullMode(D3D11_CULL_NONE);

	deltaTime = 0;
}

InstancedCloudManager::~InstancedCloudManager()
{
	SAFE_DELETE(blendState[0]);
	SAFE_DELETE(blendState[1]);

	SAFE_DELETE(rasterizerState[0]);
	SAFE_DELETE(rasterizerState[1]);

	SAFE_DELETE(clouds);

	for (int i = 0; i < whisps.size(); i++)
		SAFE_DELETE(whisps[i]);
}

void InstancedCloudManager::AddCloud(int whispCount, D3DXVECTOR3 position, float size, D3DXVECTOR3 min, D3DXVECTOR3 max, float colorMod, vector<int> whispRange)
{
	int si = 0;
	float scaleMod = Math::Distance(-min, max) / 4.5f;

	for (int w = 0; w < whispCount; w++) {
		float x = Math::Lerp(-min.x, max.x, Math::Random(0.0f, 1.0f));
		float y = Math::Lerp(-min.y, max.y, Math::Random(0.0f, 1.0f));
		float z = Math::Lerp(-min.z, max.z, Math::Random(0.0f, 1.0f));

		if (si >= 5)
			si = 0;

		rnd = Math::Random(7, 10);
		// whisp add
		whisps.push_back(new ParticleInstance(
			position + D3DXVECTOR3(x, y, z), 
			D3DXVECTOR3(1, 1, 1) * size,
			D3DXVECTOR3(whispRange[si++] / 100.0f, 1, (rnd / 10.0f) * colorMod), 
			clouds));
	}
}

void InstancedCloudManager::AddCloud(int whispCount, D3DXVECTOR3 position, float size, float radius, float colorMod, vector<int> whispRange)
{
	float si = 0;
	float scaleMode = Math::Distance(position, position * radius) / 4.5f;

	for (int w = 0; w < whispCount; w++)
	{
		float x = Math::Lerp(-radius, radius, Math::Random(0.0f, 1.0f));
		float y = Math::Lerp(-radius, radius, Math::Random(0.0f, 1.0f));
		float z = Math::Lerp(-radius, radius, Math::Random(0.0f, 1.0f));

		if (si >= whispRange.size())
			si = 0;

		// whisps add
		rnd = Math::Random(7, 10);
		whisps.push_back(new ParticleInstance(
			position + D3DXVECTOR3(x, y, z),
			D3DXVECTOR3(1, 1, 1) * size,
			D3DXVECTOR3(whispRange[si++], 1, (rnd / 10.0f) * colorMod),
			clouds));
	}
}

void InstancedCloudManager::TranslateOO(D3DXVECTOR3 distance)
{
	clouds->TranslateOO(distance);
}

void InstancedCloudManager::TranslateAA(D3DXVECTOR3 distance)
{
	clouds->TranslateAA(distance);
}

void InstancedCloudManager::Rotate(D3DXVECTOR3 axis, float angle)
{
	clouds->Rotate(axis, angle);
}

void InstancedCloudManager::Update()
{
	//deltaTime += Time::Delta();

	//if (deltaTime > 0.5f) {
	//	deltaTime = 0;
	//	SortClouds();
	//}
	// clouds Update
	clouds->Update();
}

void InstancedCloudManager::Render()
{
	// clouds Render
	rasterizerState[1]->RSSetState();
	blendState[1]->OMSetBlendState();
	clouds->Render();
	blendState[0]->OMSetBlendState();
	rasterizerState[0]->RSSetState();
}

void InstancedCloudManager::SortClouds()
{
	vector<distData> bbDists;
	list<distData> bbDists2;

	distData data;

	D3DXVECTOR3 cameraPosition;
	values->MainCamera->Position(&cameraPosition);

	for (int p = 0; p < whisps.size(); p++)
	{
		D3DXMATRIX matrix = clouds->instanceTransformMatrices[whisps[p]];
		D3DXVECTOR3 translation = D3DXVECTOR3(matrix._41, matrix._42, matrix._43);

		float dist = data.Distance(translation, cameraPosition);
		data.idx = whisps[p];
		data.dist = dist;
		bbDists.push_back(data);
	}

	sort(bbDists.begin(), bbDists.end());

	// Reorder time matrix list
	clouds->instanceTransformMatrices.clear();
	for (int p = 0; p < bbDists.size(); p++)
		clouds->instanceTransformMatrices[bbDists[p].idx] = bbDists[p].idx->world;
	
	clouds->CalcVertexBuffer();

}
