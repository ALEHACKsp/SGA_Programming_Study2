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

	// size_t usigned int 랑 같음
	for (size_t i = 0; i < bones.size(); i++) {
		ModelBone * bone = bones[i];

		if (bone->Parent() != NULL)
		{
			int index = bone->parent->index;
			// 부모의 local를 곱해야함 
			transforms[i] = bone->local * transforms[index];
		}
		else
		{
			transforms[i] = bone->local * w;
		}
	}
}

bool Model::IsPick(D3DXVECTOR3& start, D3DXVECTOR3& direction,
	OUT float& u, OUT float& v, OUT float& distance)
{
	for (ModelMesh* mesh : meshes)
	{
		ModelBone* bone = mesh->parentBone;
		D3DXMATRIX w = bone->global;

		for (ModelMeshPart* meshPart : mesh->meshParts)
		{
			for (UINT i = 0; i < meshPart->indices.size(); i += 3)
			{
				D3DXVECTOR3 p[3];
				for (UINT k = 0; k < 3; k++) {
					p[k] = meshPart->vertices[
						meshPart->indices[i + k]].Position;
					D3DXVec3TransformCoord(&p[k], &p[k], &w);
				}
				if (D3DXIntersectTri(&p[0], &p[1], &p[2],
					&start, &direction, &u, &v, &distance))
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool Model::IsPickMesh(D3DXVECTOR3 & start, D3DXVECTOR3 & direction, OUT float & u, OUT float & v, OUT float & distance,
	OUT wstring & meshName, vector<D3DXMATRIX>& boneTransforms)
{
	float tempDist;

	bool result = false;

	int index = 0;
	for (ModelMesh* mesh : meshes)
	{
		ModelBone* bone = mesh->parentBone;
		D3DXMATRIX w = boneTransforms[index];

		for (ModelMeshPart* meshPart : mesh->meshParts)
		{
			for (UINT i = 0; i < meshPart->indices.size(); i += 3)
			{
				D3DXVECTOR3 p[3];
				for (UINT k = 0; k < 3; k++) {
					p[k] = meshPart->vertices[
						meshPart->indices[i + k]].Position;
					D3DXVec3TransformCoord(&p[k], &p[k], &w);
				}
				if (D3DXIntersectTri(&p[0], &p[1], &p[2],
					&start, &direction, &u, &v, &tempDist))
				{
					if (tempDist < distance) {
						distance = tempDist;
						meshName = mesh->name;
						result = true;
					}

				}
			}
		}
		index++;
	}

	return result;
}

vector<wstring> Model::GetMesheNames()
{
	vector<wstring> meshNames;

	for (ModelMesh* mesh : meshes)
	{
		ModelBone* bone = mesh->parentBone;
		D3DXMATRIX w = bone->global;

		meshNames.push_back(mesh->name);
	}
	return meshNames;
}

void Models::Create()
{
}

void Models::Delete()
{
	for (pair<wstring, vector<Material *>> temp : materialMap)
	{
		for (Material * material : temp.second)
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

