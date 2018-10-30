#pragma once

#include "ParticleInstance.h"

class InstancedCloudManager
{
public:
	InstancedCloudManager(ExecuteValues* values);
	~InstancedCloudManager();

	void AddCloud(int whispCount, D3DXVECTOR3 position, float size, 
		D3DXVECTOR3 min, D3DXVECTOR3 max, float colorMod, vector<int> whispRange);

	void AddCloud(int whispCount, D3DXVECTOR3 position, float size,
		float radius, float colorMod, vector<int> whispRange);

	void TranslateOO(D3DXVECTOR3 distance);
	void TranslateAA(D3DXVECTOR3 distance);

	void Rotate(D3DXVECTOR3 axis, float angle);

	void Update();
	void Render();

private:
	void SortClouds();

private:
	class ParticleInstancer* clouds;

	vector<class ParticleInstance*> whisps;

	float rnd;

	Texture* cloudTexture;
	Shader* shader;

	BlendState* blendState[2];

	ExecuteValues* values;
public:
	struct distData
	{
		float dist;
		ParticleInstance* idx;

		distData() {}
		distData(ParticleInstance* idx, float dist) {
			this->idx = idx;
			this->dist = dist;
		}

		int Compare(distData x, distData y) {
			return (int)(y.dist - x.dist);
		}

		float Distance(D3DXVECTOR3 v1, D3DXVECTOR3 v2) {
			D3DXVECTOR3 unit = v2 - v1;
			float distance = D3DXVec3Length(&unit);

			return distance *= distance;
		}
	};
};