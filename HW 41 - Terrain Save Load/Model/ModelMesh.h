#pragma once

#include "Interfaces\ICloneable.h"

class ModelMesh : ICloneable
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
};