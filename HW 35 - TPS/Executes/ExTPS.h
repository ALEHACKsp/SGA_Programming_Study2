#pragma once
#include "Execute.h"
#include "../Fbx/Exporter.h"

class ExTPS : public Execute
{
public:
	ExTPS(ExecuteValues* values);
	~ExTPS();

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

	Ray* playerRay;
	float rayLife;
	D3DXVECTOR3 rayPos, rayDir;

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

	class Sky* sky;

	Shader* planeShader;
	class MeshPlane* plane;
	
	class MeshSphere* sphere;

	Camera* free;
	class SmoothFollow* smoothFollow;

	class CapsuleCollider* capsule;
	bool bCapsuleCollision;

};