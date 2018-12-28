#pragma once

enum ObjType
{
	ObjType_Cube,
	ObjType_Sphere,
	ObjType_Tree,
	ObjType_End
};
class GameModel;

typedef GameModel* (*CreateObjCallback)(ExecuteValues* values, D3DXVECTOR3 position, D3DXVECTOR3 scale);
extern CreateObjCallback CreateObjectType[ObjType_End];

GameModel* CreateCube(ExecuteValues* values, D3DXVECTOR3 position, D3DXVECTOR3 scale = D3DXVECTOR3(1, 1, 1));
GameModel* CreateSphere(ExecuteValues* values, D3DXVECTOR3 position, D3DXVECTOR3 scale = D3DXVECTOR3(1, 1, 1));
GameModel* CreateTree(ExecuteValues* values, D3DXVECTOR3 position, D3DXVECTOR3 scale = D3DXVECTOR3(1, 1, 1));