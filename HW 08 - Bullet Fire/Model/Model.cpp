#include "stdafx.h"
#include "Model.h"

#include "ModelBone.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"

Model::Model()
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

	// size_t usigned int ¶û °°À½
	for (size_t i = 0; i < bones.size(); i++) {
		ModelBone * bone = bones[i];

		if (bone->Parent() != NULL)
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

void Model::MinMaxVertex(D3DXVECTOR3 * min, D3DXVECTOR3 * max)
{
	D3DXVECTOR3 minVec(FLT_MAX, FLT_MAX, FLT_MAX),
		maxVec(FLT_MIN, FLT_MIN, FLT_MIN);

	for (ModelMesh* mesh : meshes)
	{
		ModelBone* bone = mesh->parentBone;
		D3DXMATRIX w = bone->global;

		for (ModelMeshPart* meshPart : mesh->meshParts)
		{
			for (UINT i = 0; i < meshPart->vertices.size(); i++)
			{
				D3DXVECTOR3 pos = meshPart->vertices[i].Position;
				D3DXVec3TransformCoord(&pos, &pos, &w);

				if (minVec.x > pos.x) minVec.x = pos.x;
				if (minVec.y > pos.y) minVec.y = pos.y;
				if (minVec.z > pos.z) minVec.z = pos.z;

				if (maxVec.x < pos.x) maxVec.x = pos.x;
				if (maxVec.y < pos.y) maxVec.y = pos.y;
				if (maxVec.z < pos.z) maxVec.z = pos.z;

			}
		}
	}

	*min = minVec;
	*max = maxVec;
}

void Models::Create()
{
}

void Models::Delete()
{
}
