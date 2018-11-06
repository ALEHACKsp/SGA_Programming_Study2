#include "stdafx.h"
#include "ModelBone.h"

ModelBone::ModelBone()
	:parent(NULL)
{
}

ModelBone::~ModelBone()
{
}

ModelBone::ModelBone(wstring name, int parentIndex, ModelBone * parent, D3DXMATRIX local, D3DXMATRIX global)
	: name(name), parent(parent), local(local), global(global)
{
}

void ModelBone::Clone(void ** clone)
{
	ModelBone* bone = new ModelBone();
	bone->index = index;
	bone->name = name;
	bone->parentIndex = parentIndex;
	bone->local = local;
	bone->global = global;

	*clone = bone;
}
