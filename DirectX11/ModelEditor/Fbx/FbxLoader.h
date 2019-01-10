#pragma once

// fbx������ ���ϴ� ���·� �����ϴ� Ŭ����

namespace Xml
{
	class XMLDocument;
	class XMLElement;
}

class FbxLoader
{
public:
	FbxLoader(wstring file, wstring saveFolder, wstring saveName);
	~FbxLoader();

	// fbx���� ���� ���� �����ؼ� �����ϴ� �Լ�
	void ExportMaterial(wstring saveFolder = L"", wstring fileName = L"");
	void ExportMesh(wstring saveFolder = L"", wstring fileName = L"");

	void GetClipList(vector<wstring>* list);
	void ExportAnimation(UINT clipNumber, wstring saveFolder = L"", wstring fileName = L"");
	void ExportAnimation(wstring clipName, wstring saveFolder = L"", wstring fileName = L"");

private:
	void ReadMaterial();
	void WriteMaterial(wstring saveFolder, wstring fileName);

	void ReadBoneData(FbxNode* node, int index, int parent);
	UINT GetBoneIndexByName(string name);
	void ReadMeshData(FbxNode* node, int parentBone);
	void ReadSkinData();
	void WriteMeshData(wstring saveFolder, wstring fileName);

	struct FbxClip* ReadAnimationData(UINT index);
	struct FbxClip* ReadAnimationData(wstring name);
	void ReadKeyframeData(FbxClip * clip, FbxNode * node, int start, int stop);

	void WriteClipData(struct FbxClip* clip, wstring saveFolder, wstring fileName);

private:
	FbxManager* manager;
	FbxImporter* importer;
	FbxScene* scene;
	FbxIOSettings* ios;

private:
	void WriteXmlColor(Xml::XMLDocument* document,
		Xml::XMLElement* element, D3DXCOLOR& color);

	// OUT �ǹ̴� ���µ� ���ϵ� ������� ��ŷ�� ���ִ°�
	void CopyTextureFile(OUT string& textureFile, wstring& saveFolder);

private:
	wstring fbxFile;
	wstring saveFolder;
	wstring saveName;

	vector<struct FbxMaterial *> materials;

	vector<struct FbxBoneData *> boneDatas;
	vector<struct FbxMeshData *> meshDatas;

	unordered_map<UINT, struct FbxControlPointData> cpDatas;
};


