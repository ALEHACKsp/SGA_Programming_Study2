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

	void SaveAnimation(wstring file);

private:
	int selectModel;
	int selectClip;
	int selectModelClip;
	bool clipSelect;

	vector<class GameAnimModel*> models;
	vector<class ModelClip*> clips;

	Shader* shader;

	function<void(wstring)> func;

	Fbx::Exporter* exporter;
	wstring file;
	char animationFile[32];
	bool isExportOption;
	bool isExportMaterial;
	bool isExportMesh;
	bool isExportAnimation;
	bool isExportRead;

	bool imGuiWindowNoMove;
	bool showAllBoneTree;

	int selectMode;

	bool bLockRoot;
	bool bRepeat;
	float clipSpeed;
	float startTime;

};