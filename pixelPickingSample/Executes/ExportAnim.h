#pragma once
#include "Execute.h"

class ExportAnim : public Execute
{
public:
	ExportAnim(ExecuteValues* values);
	~ExportAnim();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	class Shader* specular;
	D3DXCOLOR diffuseColor;
	D3DXCOLOR specularColor;
	float shininess;

	class PickingObject* pickingObj;
	class Terrain* terrain;
	class Sky* sky;
	vector<class GameModel*> objects;
	class MeshSphere* sphere;
	class MeshCube* cube;
	class PointLight* pointLight;
	class Shader* pointLighting;
	class SpotLight* spotLight;
	class Shader* spotLighting;

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
};