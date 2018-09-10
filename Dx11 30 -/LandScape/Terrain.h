#pragma once

class Terrain
{
public:
	Terrain(ExecuteValues* values, Material* material);
	~Terrain();

	UINT VertexCount() { return vertexCount; }
	void CopyVertices(void* vertices);

private:
	void ReadHeightMap(wstring file);
	void CalcNormalVector();
	void AlignVertexData();

private:
	ExecuteValues * values;
	
	UINT width, height;

	VertexTextureNormal * vertices;
	UINT* indices;

	UINT vertexCount, indexCount;
};