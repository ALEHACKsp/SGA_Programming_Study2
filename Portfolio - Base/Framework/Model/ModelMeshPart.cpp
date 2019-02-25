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
	// ���⼭ �Ǵ°��� �ϴ� �𸣰���
	//if (boneIndexVariable == NULL)
	//	boneIndexVariable = material->GetShader()->AsScalar("BoneIndex");

	//boneIndexVariable->SetInt(parent->ParentBoneIndex());
	//material->GetShader()->Draw(0, pass, vertexCount, startVertex);
	material->GetShader()->DrawIndexed(0, pass, indexCount, startIndex);
}

void ModelMeshPart::RenderInstance(UINT count)
{
	if (boneIndexVariable == NULL)
		boneIndexVariable = material->GetShader()->AsScalar("BoneIndex");

	boneIndexVariable->SetInt(parent->ParentBoneIndex());
	//material->GetShader()->DrawInstanced(0, pass, vertexCount, count, startVertex);
	material->GetShader()->DrawIndexedInstanced(0, pass, indexCount, count, startIndex);

}

void ModelMeshPart::Clone(void ** clone)
{
	ModelMeshPart* part = new ModelMeshPart();
	part->name = name;

	part->materialName = materialName;

	part->vertexCount = vertexCount;
	part->startVertex = startVertex;

	part->indexCount = indexCount;
	part->startIndex = startIndex;

	*clone = part;
}