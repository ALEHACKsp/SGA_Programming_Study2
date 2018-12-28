#include "stdafx.h"
#include "Model.h"
#include "ModelBone.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"

Model::Model()
	: bInstance(false)
{
	buffer = new BoneBuffer();
}

Model::~Model()
{
	SAFE_DELETE(buffer);

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

//world matrix가 변하면 Bone도 함께 변화
void Model::CopyGlobalBoneTo(vector<D3DXMATRIX>& transforms)
{
	D3DXMATRIX w;
	D3DXMatrixIdentity(&w);

	CopyGlobalBoneTo(transforms, w);
}

void Model::CopyGlobalBoneTo(vector<D3DXMATRIX>& transforms, D3DXMATRIX& w)
{
	transforms.clear();
	transforms.assign(bones.size(), D3DXMATRIX());

	for (size_t i = 0; i < bones.size(); i++)
	{
		ModelBone* bone = bones[i];

		if (bone->parent != NULL)
		{
			int index = bone->parent->index;
			transforms[i] = bone->local * transforms[index];
		}
		else
		{
			transforms[i] = bone->local * w;
		}
	}
}

void Model::CopyGlobalBoneToName(wstring name, D3DXMATRIX* boneOut)
{
	ModelBone* bone = BoneByName(name);
	*boneOut = bone->global * bone->local;
	while (bone->parent != NULL)
	{
		*boneOut = *boneOut * bone->parent->local;
		bone = bone->parent;
	}
	
}

void Model::SetInstance(ID3D11Buffer * instanceBuffer, UINT instanceCount)
{
	if (bInstance == false) {
		bInstance = true;
		for (int i = 0; i < 128; i++)
			instanceBuffers[i] = new InstanceBoneBuffer();
	}

	for (ModelMesh* mesh : meshes)
	{
		for (ModelMeshPart* part : mesh->meshParts)
		{
			part->bInstance = true;

			part->instanceBuffer = instanceBuffer;
			part->instanceCount = instanceCount;
		}
	}
}

void Model::SetInstanceBuffer(vector<D3DXMATRIX>& instance, int index)
{
	instanceBuffers[index]->SetInstance(instance);
}

void Models::Craete()
{
}

void Models::Delete()
{
	for (pair<wstring, vector<Material *>> temp : materialMap)
	{
		for (Material* material : temp.second)
			SAFE_DELETE(material);
	}

	for (pair<wstring, MeshData> temp : meshDataMap)
	{
		MeshData data = temp.second;

		for (ModelBone* bone : data.Bones)
			SAFE_DELETE(bone);

		for (ModelMesh* mesh : data.Meshes)
			SAFE_DELETE(mesh);
	}
}