#include "Framework.h"
#include "ModelMeshPart.h"
#include "ModelMesh.h"
#include "ModelBone.h"

ModelMeshPart::ModelMeshPart()
	: pass(0)
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
	material->GetShader()->Draw(0, pass, vertexCount, startVertex);
}

void ModelMeshPart::Clone(void ** clone)
{
	ModelMeshPart* part = new ModelMeshPart();
	part->materialName = materialName;
	part->vertexCount = vertexCount;
	part->startVertex = startVertex;

	*clone = part;
}