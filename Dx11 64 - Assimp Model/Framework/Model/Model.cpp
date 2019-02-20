#include "Framework.h"
#include "Model.h"

#include "ModelBone.h"
#include "ModelMesh.h"

Model::Model()
{

}

Model::~Model()
{
	for (Material* material : materials)
		SAFE_DELETE(material);
	
	for (ModelBone* bone : bones)
		SAFE_DELETE(bone);

	for (ModelMesh* mesh : meshes)
		SAFE_DELETE(mesh);
}

Material * Model::MaterialByName(wstring name)
{
	for (Material* material : materials)
	{
		if (material->Name() == name)
			return material;
	}

	return NULL;
}

ModelBone * Model::BoneByName(wstring name)
{
	for (ModelBone* bone : bones)
	{
		if (bone->Name() == name)
			return bone;
	}

	return NULL;
}

ModelMesh * Model::MeshByName(wstring name)
{
	for (ModelMesh* mesh : meshes)
	{
		if (mesh->Name() == name)
			return mesh;
	}

	return NULL;
}

void Models::Create()
{
}

void Models::Delete()
{
	map<wstring, vector<Material *>>::iterator matIter;
	for (matIter = materialMap.begin(); matIter != materialMap.end(); matIter++)
	{
		for (Material * material : matIter->second)
			SAFE_DELETE(material);
	}

	map<wstring, MeshData>::iterator meshIter;
	for (meshIter = meshDataMap.begin(); meshIter != meshDataMap.end(); meshIter++)
	{
		for (ModelBone* bone : meshIter->second.Bones)
			SAFE_DELETE(bone);

		for (ModelMesh* mesh : meshIter->second.Meshes)
			SAFE_DELETE(mesh);
	}
}

