#pragma once
#include "MapToolCommand.h"

//�ڷ� ���� ������ �����͸� �����ڸ� ���� ����, �ڷ� ���� �Ĵ� ChangeData �Լ��� ���� ����
class GameModel;
class MapToolDrawLand : public MapToolCommand
{
public:
	MapToolDrawLand(VertexTextureNormalTangentSplatting* vertices, UINT vertexCount);
	~MapToolDrawLand();

	void ChangeData();
	void Excute();
	void Undo();

private:
	VertexTextureNormalTangentSplatting* vertices;
	UINT vertexCount;

	VertexTextureNormalTangentSplatting* verticesBefore;
	VertexTextureNormalTangentSplatting* verticesAfter;
};

class MapToolCreateObejct : public MapToolCommand
{
public:
	MapToolCreateObejct(vector<GameModel*>* objects);
	~MapToolCreateObejct();

	void ChangeData();
	void Excute();
	void Undo();

private:
	GameModel* object;
	vector<GameModel*>* objects;
	vector<GameModel*>::iterator iter;
	bool bIsUndo;
};

class MapToolDeleteObejct : public MapToolCommand
{
public:
	MapToolDeleteObejct(vector<GameModel*>* objects, UINT index);

	void ChangeData();
	void Excute();
	void Undo();
private:
	UINT index;
	GameModel* object;
	vector<GameModel*>* objects;
};

class MapToolMoveObejct : public MapToolCommand
{
public:
	MapToolMoveObejct(GameModel* object);

	void ChangeData();
	void Excute();
	void Undo();
private:
	GameModel* object;

	D3DXVECTOR3 positionBefore;
	D3DXVECTOR3 scaleBefore;
	D3DXVECTOR3 rotationBefore;

	D3DXVECTOR3 positionAfter;
	D3DXVECTOR3 scaleAfter;
	D3DXVECTOR3 rotationAfter;
};