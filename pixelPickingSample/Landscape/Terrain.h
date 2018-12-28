#pragma once

class Terrain
{
public:
	Terrain(wstring heightMapFile);
	~Terrain();

	UINT VertexCount() { return vertexCount; }
	void CopyVertices(void* vertices);

	Texture* HeightMap() { return heightMap; }
private:
	void ReadHeightMap(wstring file);
	void CalcNormalVector();
	void AlignVertexData();

private:
	Texture* heightMap;

	UINT width, height;

	VertexTextureNormal * vertices;
	UINT* indices;

	UINT vertexCount, indexCount;
};