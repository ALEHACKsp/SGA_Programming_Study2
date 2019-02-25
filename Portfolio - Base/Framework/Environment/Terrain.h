#pragma once

class Terrain
{
public:
	struct InitDesc;

public:
	Terrain(InitDesc& desc);
	~Terrain();

	void Initialize();
	void Ready(Material* material);

	void Update();
	void PreRender();
	void Render();

	void Data(UINT row, UINT col, float data);
	float Data(UINT row, UINT col);

	float Width();
	float Depth();

	float GetHeight(float x, float z);

	class HeightMap* GetHeightMap() { return heightMap; }

public:
	static const UINT CellsPerPatch;

private:
	class TerrainRender* render;
	class HeightMap* heightMap;

public:
	struct InitDesc
	{
		wstring HeightMap;
		float HeightScale; // 높이에 대한 비율값
		float HeightMapWidth;
		float HeightMapHeight;

		wstring layerMapFile[5];
		wstring blendMapFile;
		float CellSpacing; // 한 셀의 크기

		Material* material;

		// 랜덤 지형의 쓰는 것
		//float NoiseSize1;
		//float NoiseSize2;
		//float Persistence1;
		//float Persistence2;

		//int Overcast1;
		//int Overcast2;
		//int Seed;
	};
private:
	InitDesc desc;

public:
	InitDesc& Desc() { return desc; }
};