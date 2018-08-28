#pragma once
#include "Execute.h"

class DrawLandScape : public Execute
{
public:
	DrawLandScape(ExecuteValues* values);
	~DrawLandScape();

	// Execute��(��) ���� ��ӵ�
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	//class Shader* specular;
	//
	//D3DXCOLOR diffuseColor;
	//D3DXCOLOR specularColor;
	//float shininess;
	//class MeshSphere* sphere;

	class Sky* sky;
	class Terrain* terrain;

	int selectModel;
	vector<class GameModel*> models;

	function<void(wstring)> func;
};