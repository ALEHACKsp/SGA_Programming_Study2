#pragma once

class MapToolData
{
public:
	MapToolData();
	~MapToolData();

	void SetRender();

	void SaveMapData();
	void LoadMapData();

	void LoadHeightMap(wstring fileName);

private:
	void CreateVertexData(wstring fileName);
	void CreateNormalData();
	void CreateTangentData();

public:
	WorldBuffer* worldBuffer;

	UINT width, height;

	VertexTextureNormalTangentSplatting* vertices;
	UINT* indices;

	UINT vertexCount, indexCount, textureCount;

	wstring texFileName[5];
};