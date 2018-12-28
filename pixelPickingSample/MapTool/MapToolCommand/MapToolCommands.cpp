#include "stdafx.h"
#include "MapToolCommands.h"
#include "./Objects/GameModel.h"

MapToolDrawLand::MapToolDrawLand(VertexTextureNormalTangentSplatting * vertices, UINT vertexCount)
	: vertices(vertices), vertexCount(vertexCount)
{
	//Create Before VertexData
	{
		this->verticesBefore = new VertexTextureNormalTangentSplatting[vertexCount];
		this->verticesAfter = new VertexTextureNormalTangentSplatting[vertexCount];

		memcpy(this->verticesBefore, vertices, sizeof(VertexTextureNormalTangentSplatting) * vertexCount);
	}
}

MapToolDrawLand::~MapToolDrawLand()
{
	SAFE_DELETE_ARRAY(verticesBefore);
	SAFE_DELETE_ARRAY(verticesAfter);
}

void MapToolDrawLand::ChangeData()
{
	//Create After VertexData
	memcpy(this->verticesAfter, vertices, sizeof(VertexTextureNormalTangentSplatting) * vertexCount);
}

void MapToolDrawLand::Excute()
{
	memcpy(vertices, this->verticesAfter, sizeof(VertexTextureNormalTangentSplatting) * vertexCount);
}

void MapToolDrawLand::Undo()
{
	memcpy(vertices, this->verticesBefore, sizeof(VertexTextureNormalTangentSplatting) * vertexCount);
}

MapToolCreateObejct::MapToolCreateObejct(vector<GameModel*>* objects)
	: objects(objects), object(NULL)
{

}

MapToolCreateObejct::~MapToolCreateObejct()
{
	if (bIsUndo)
		SAFE_DELETE(object);
}

void MapToolCreateObejct::ChangeData()
{
}

void MapToolCreateObejct::Excute()
{
	objects->push_back(object);
	bIsUndo = false;
}

void MapToolCreateObejct::Undo()
{
	iter = objects->end() - 1;
	object = (*iter);
	objects->erase(iter);
	bIsUndo = true;
}

MapToolMoveObejct::MapToolMoveObejct(GameModel * object)
	: object(object)
{
	positionBefore = object->Position();
	scaleBefore = object->Scale();
	rotationBefore = object->Rotation();
}

void MapToolMoveObejct::ChangeData()
{
	positionAfter = object->Position();
	scaleAfter = object->Scale();
	rotationAfter = object->Rotation();
}

void MapToolMoveObejct::Excute()
{
	object->Position(positionAfter);
	object->Scale(scaleAfter);
	object->Rotation(rotationAfter);
}

void MapToolMoveObejct::Undo()
{
	object->Position(positionBefore);
	object->Scale(scaleBefore);
	object->Rotation(rotationBefore);
}
