#pragma once
#include "Execute.h"

class TestFlyweight : public Execute
{
public:
	TestFlyweight(ExecuteValues* values);
	~TestFlyweight();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

	void CreateInstanceBuffer();

private:
	Shader* shader;
	Shader* instanceShader;

	vector<class GameModel*> models;

	class GameModel* instance;

	vector< vector<D3DXMATRIX> > worlds;

	ID3D11Buffer* instanceBuffer;
	UINT instanceCount;


	bool bInstance;
	bool bRotate;
	
	class Sky* sky;

private:
	struct InstanceBuffer
	{
		int Id;
	};
};