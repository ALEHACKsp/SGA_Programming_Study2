#pragma once
#include "Execute.h"
#include "../Fbx/Exporter.h"

class TestLookAt : public Execute
{
public:
	TestLookAt(ExecuteValues* values);
	~TestLookAt();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	vector<class GameAnimModel*> models;

	Shader* shader;

	function<void(wstring)> func;

	int selectModel;

private:
	class VisibleBuffer : public ShaderBuffer
	{
	public:
		VisibleBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.VisibleNormal = 1;
			Data.VisibleSpecular = 1;
		}

		struct Struct
		{
			int VisibleNormal;
			int VisibleSpecular;
			
			float Padding[2];
		} Data;
	};

	VisibleBuffer* visibleBuffer;

	Shader* planeShader;
	class MeshPlane* plane;
	
	bool bLookAt;
	bool bOrbit;

	Camera* free;
	class Orbit* orbit;
};