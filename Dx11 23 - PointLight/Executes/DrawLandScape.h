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
	class Shader* specular;
	class Shader* normalMap;
	class Shader* pointLighting;

	D3DXCOLOR diffuseColor;
	D3DXCOLOR specularColor;
	float shininess;

	class Sky* sky;
	class Terrain* terrain;
	class MeshSphere* sphere;
	class MeshCube* cube;
	vector<class Billboard*> trees;

	class PointLight* pointLight;

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