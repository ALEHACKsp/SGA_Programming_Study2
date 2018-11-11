#pragma once
#include "Execute.h"
#include "../Fbx/Exporter.h"

class ExCharacterTool : public Execute
{
public:
	ExCharacterTool(ExecuteValues* values);
	~ExCharacterTool();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

	void OpenFbx(wstring path);
	void ExportMaterial();
	void ExportMesh();
	void ExportAnimation(wstring file);

	void ReadModel(wstring file);
	void ReadAnimation(wstring file);

	void SaveModel(wstring file);
	void SaveAnimation(wstring file);

private:
	vector<class GameAnimModel*> models;
	vector<class ModelClip*> clips;

	Shader* shader;

	function<void(wstring)> func;

	Fbx::Exporter* exporter;
	wstring file;

	class ModelBone* addBone;
	char nameBuf[16];
	DepthStencilState* depthMode[2];
	class DebugDrawCube* debug;

	D3DXVECTOR3 boneScale;
	D3DXVECTOR3 boneRotation;
	D3DXVECTOR3 bonePosition;

	char animationFile[64];
	bool isExportOption;
	bool isExportMaterial;
	bool isExportMesh;
	bool isExportAnimation;
	bool isExportRead;

	bool isAnimMode;
	bool bLockRoot;
	bool bRepeat;
	float clipSpeed;
	float startTime;

	class Camera* freedomCamera;
	class Orbit* orbitCamera;
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
};