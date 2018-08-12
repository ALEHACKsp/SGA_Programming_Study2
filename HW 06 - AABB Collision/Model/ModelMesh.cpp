#include "stdafx.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"
#include "ModelBone.h"

ModelMesh::ModelMesh()
{
	worldBuffer = new WorldBuffer();
}

ModelMesh::~ModelMesh()
{
	for (ModelMeshPart* part : meshParts)
		SAFE_DELETE(part);

	SAFE_DELETE(worldBuffer);
}

void ModelMesh::Render()
{
	worldBuffer->SetVSBuffer(1);

	for (ModelMeshPart* part : meshParts)
	{
		part->Render();
	}
}

void ModelMesh::Binding()
{
	for (ModelMeshPart* part : meshParts)
		part->Binding();
}