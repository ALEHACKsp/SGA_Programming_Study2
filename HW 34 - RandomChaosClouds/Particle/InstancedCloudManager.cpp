#include "stdafx.h"
#include "InstancedCloudManager.h"

#include "ParticleInstancer.h"

InstancedCloudManager::InstancedCloudManager(ExecuteValues* values)
	:values(values)
{
	clouds = new ParticleInstancer();

	cloudTexture = new Texture(Textures + L"CloudSpriteSheetOrg.png");

	// shader
	
	blendState[0] = new BlendState();
	// NonePremultiplied
	blendState[1] = new BlendState();

	
	
}

InstancedCloudManager::~InstancedCloudManager()
{
	SAFE_DELETE(blendState[0]);
	SAFE_DELETE(blendState[1]);

	SAFE_DELETE(cloudTexture);

	SAFE_DELETE(shader);

	SAFE_DELETE(clouds);
}

void InstancedCloudManager::AddCloud(int whispCount, D3DXVECTOR3 position, float size, D3DXVECTOR3 min, D3DXVECTOR3 max, float colorMod, vector<int> whispRange)
{
	int si = 0;
	float scaleMod = Math::Distance(-min, max) / 4.5f;

	for (int w = 0; w < whispCount; w++) {
		float x = Math::Lerp(-min.x, max.x, rnd);
		float y = Math::Lerp(-min.y, max.y, rnd);
		float z = Math::Lerp(-min.z, max.z, rnd);

		if (si >= whispRange.size())
			si = 0;

		// whisp add
		rnd = Math::Random(7, 10);
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
		float x = Math::Lerp(-radius, radius, rnd);
		float y = Math::Lerp(-radius, radius, rnd);
		float z = Math::Lerp(-radius, radius, rnd);

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
	SortClouds();
	// clouds Update
}

void InstancedCloudManager::Render()
{
	// clouds Render
}

void InstancedCloudManager::SortClouds()
{
	vector<distData> bbDists;

	distData data;

	for (int p = 0; p < whisps.size(); p++)
	{
		D3DXMATRIX matrix = clouds->instanceTransformMatrices[whisps[p]];
		D3DXVECTOR3 translation = D3DXVECTOR3(matrix._41, matrix._42, matrix._43);

		D3DXVECTOR3 cameraPosition;
		values->MainCamera->Position(&cameraPosition);

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
