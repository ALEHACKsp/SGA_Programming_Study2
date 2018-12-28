#pragma once

enum class CloudType
{
	SpotClouds,
	CloudySky,
	CloudField,
	CloudSplatter
};

class Cloud
{
public:
	Cloud(ExecuteValues* values, CloudType CloudType);
	~Cloud();

	void Update();
	void Render();

private:
	void GenerateCloud(CloudType CloudType);
	void AddCloud(int whispCount, D3DXVECTOR3 position, float size, D3DXVECTOR3 min, D3DXVECTOR3 max, float colorMod, UINT* whispRange, UINT whispRangeNum);
	void AddCloud(int whispCount, D3DXVECTOR3 position, float size, float radius, float colorMod, UINT* whispRange, UINT whispRangeNum);
	void RotateCloud(float degreePerSec);

private:
	ExecuteValues* values;
	class Particles* clouds;
};