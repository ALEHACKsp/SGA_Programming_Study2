#pragma once
#include "Execute.h"

class DrawLandScape : public Execute
{
public:
	DrawLandScape(ExecuteValues* values);
	~DrawLandScape();

	// Execute을(를) 통해 상속됨
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

	class PointLight* pointLight;
	class SpotLight* spotLight;
	class AreaLight* areaLight;

	D3DXVECTOR3 pickPos;

private:
	struct IsRandomMove
	{
		bool isMove;
		D3DXVECTOR3 dir;
		float liveTime;
	};

	vector<IsRandomMove> isRandomMoves;
	// min max xz Range 
	float moveRange[4];
private:
	class NormalBuffer : public ShaderBuffer
	{
	public:
		NormalBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.VisibleNormal = 1;
		}

		struct Struct
		{
			int VisibleNormal;

			float Padding[3];
		} Data;
	};

	NormalBuffer* normalBuffer;
};