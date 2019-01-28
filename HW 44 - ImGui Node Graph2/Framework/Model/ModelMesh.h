#pragma once

class ModelMesh
{
public:
	friend class Model;
	friend class Models;

public:
	void PreRender();
	void Render();

	wstring Name() { return name; }

	int ParentBoneIndex() { return parentBoneIndex; }
	class ModelBone* ParentBone() { return parentBone; }

	vector<class ModelMeshPart *>& MeshParts() { return meshParts; }

	ID3D11Buffer* VertexBuffer() { return vertexBuffer; }
	VertexTextureNormalTangentBlend* Vertices() { return vertices; }
	UINT VertexCount() { return vertexCount; }

public:
	void Clone(void ** clone);

	void Pass(UINT val);

private:
	void Binding();

private:
	ModelMesh();
	~ModelMesh();

	wstring name;

	int parentBoneIndex;
	class ModelBone* parentBone;

	vector<class ModelMeshPart *> meshParts;

	UINT vertexCount;
	VertexTextureNormalTangentBlend* vertices;
	ID3D11Buffer* vertexBuffer;
};