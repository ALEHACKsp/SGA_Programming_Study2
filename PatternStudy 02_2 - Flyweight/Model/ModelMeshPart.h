#pragma once

#include "../Interfaces/ICloneable.h"

class ModelMeshPart : public ICloneable
{
public:
	friend class Model;
	friend class Models;
	friend class ModelMesh;

private:
	ModelMeshPart();
	~ModelMeshPart();

	void Binding();
	void Render();

	void CreateInstanceBuffer();

public:
	void Clone(void ** clone);

private:
	Material* material;
	wstring materialName;

	class ModelMesh* parent;

	vector<VertexTextureNormalTangentBlend> vertices;
	vector<UINT> indices;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	ID3D11Buffer* instanceBuffer;
	UINT instanceCount;

	bool bInstance;
};