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
}

ModelBone * Model::BoneByName(wstring name)
{
	for (ModelBone* bone : bones)
	{
		if (bone->Name() == name)
			return bone;
	}
}

ModelMesh * Model::MeshByName(wstring name)
{
	for (ModelMesh* mesh : meshes)
	{
		if (mesh->Name() == name)
			return mesh;
	}
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

void Model::Clone(void ** clone)
{
	*clone = new Model();
	((Model*)*clone)->CloneMaterial(this->Materials());
	((Model*)*clone)->CloneMesh(Bones(), Meshes());
}

void Model::CloneMaterial(vector<Material*> materials)
{
	for (Material* material : materials) {
		Material* clone = new Material();
		clone->Name(material->Name());
		D3DXCOLOR color = *material->GetDiffuse();
		clone->SetDiffuse(color);
		Texture* texture = material->GetDiffuseMap();
		if(texture != NULL)
			clone->SetDiffuseMap(texture->GetFile());
		this->materials.push_back(clone);
	}
}

void Model::CloneMesh(vector<ModelBone*> bones, vector<ModelMesh*> meshes)
{
	for (ModelBone* bone : bones) 
	{
		ModelBone * cloneBone = new ModelBone();

		cloneBone->index = bone->Index();
		cloneBone->name = bone->Name();
		cloneBone->parentIndex = bone->ParentIndex();

		cloneBone->local = bone->Local();
		cloneBone->global = bone->Global();

		this->bones.push_back(cloneBone);
	} // for(bone)

	for (ModelMesh* mesh : meshes)
	{
		ModelMesh* cloneMesh = new ModelMesh();
		cloneMesh->name = mesh->Name();
		cloneMesh->parentBoneIndex = mesh->ParentBoneIndex();

		for (ModelMeshPart * meshPart : mesh->meshParts)
		{
			ModelMeshPart* cloneMeshPart = new ModelMeshPart();
			cloneMeshPart->parent = cloneMesh;
			cloneMeshPart->materialName = meshPart->materialName;

			cloneMeshPart->vertices = meshPart->vertices;
			cloneMeshPart->indices = meshPart->indices;

			cloneMesh->meshParts.push_back(cloneMeshPart);
		} // for (meshPart)
		this->meshes.push_back(cloneMesh);
	} // for (mesh)

	BindingBone();
	BindingMesh();
}
