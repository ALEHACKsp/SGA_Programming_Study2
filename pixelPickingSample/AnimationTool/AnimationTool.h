#pragma once
#include "./Fbx/Exporter.h"

class AnimationTool
{
public:
	AnimationTool(ExecuteValues* values);
	~AnimationTool();

	void Update();
	void Render();
	void PostRender();

private:
	void LoadDataList();

	void LoadFbxFile(wstring fileName);
	void RenderFbxCheckBox();
	void ExportFbxFile();

	void SaveAnimation(wstring fileName);

	void SelectModel();
	void RenderBone();
	void RenderAnimationEditor();
	void RenderAnimationRetargeting();

	void RenderBoneNames(class ModelBone* bone);
private:
	vector<string> materials;
	vector<string> meshes;
	vector<string> animations;

	int selectMaterial;
	int selectMesh;
	int selectAnimation;

	bool bShowBones;
	bool bLockRoot;
	bool bRepeat;
	UINT interpolateFrame;
	
	bool bRetargeting;
	wstring boneNameBuf;

	Fbx::Exporter* exporter;
	bool bExport[3];
	bool bExportCheck[32];
	vector<string> animNames;
	vector<string> exportNames;
	wstring fileName;

	class GameAnimModel* selectModel;
	class MeshSphere* sphere;

	class PickingObject* pickingObj;

	ExecuteValues* values;
};