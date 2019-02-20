#include "Framework.h"
#include "ModelMeshPart.h"
#include "ModelMesh.h"
#include "ModelBone.h"

ModelMeshPart::ModelMeshPart()
	: pass(0), boneIndexVariable(NULL)
{

}

ModelMeshPart::~ModelMeshPart()
{

}

void ModelMeshPart::PreRender()
{

}

void ModelMeshPart::Render()
{
	// 여기서 되는건지 일단 모르겠음
	//if (boneIndexVariable == NULL)
	//	boneIndexVariable = material->GetShader()->AsScalar("BoneIndex");

	//boneIndexVariable->SetInt(parent->ParentBoneIndex());
	material->GetShader()->Draw(0, pass, vertexCount, startVertex);
}

void ModelMeshPart::RenderInstance(UINT count)
{
	if (boneIndexVariable == NULL)
		boneIndexVariable = material->GetShader()->AsScalar("BoneIndex");

	boneIndexVariable->SetInt(parent->ParentBoneIndex());
	material->GetShader()->DrawInstanced(0, pass, vertexCount, count, startVertex);
}

void ModelMeshPart::Clone(void ** clone)
{
	ModelMeshPart* part = new ModelMeshPart();
	part->materialName = materialName;
	part->vertexCount = vertexCount;
	part->startVertex = startVertex;

	*clone = part;
}