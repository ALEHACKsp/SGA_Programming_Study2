#pragma once

namespace Xml
{
	class XMLDocument;
	class XMLElement;
}

class Loader
{
public:
	Loader(wstring file, wstring saveFolder, wstring saveName);
	~Loader();

	void ExportMaterial(wstring saveFolder = L"", wstring fileName = L"");
	void ExportMesh(wstring saveFolder = L"", wstring fileName = L"");

	void ExportAnimation(wstring saveFloder = L"", wstring fileName = L"");

private:
	void ReadMaterial();
	void WriteMaterial(wstring saveFolder, wstring fileName);

	void ReadBoneData(aiNode* node, int index, int parent);
	void ReadSkinData();
	void ReadMeshData(aiNode* node, int parentBone);

	void WriteMeshData(wstring saveFolder, wstring fileName);

	struct AsClip* ReadAnimationData(UINT index);

	void WriteClipData(struct AsClip* clip, wstring saveFolder, wstring fileName);

private:
	void WriteXmlColor(Xml::XMLDocument* document, Xml::XMLElement* element, D3DXCOLOR& color);
	string WriteTexture(string file);

	UINT FindBoneIndex(string name);

	void ReadKeyframeData(AsClip* clip, aiNode* node, vector<struct AsAniNode>& aniNodeInfos);
	struct AsAniNode* FindAniNode(vector<struct AsAniNode>& aniNodeInfos, aiString name);

private:
	Assimp::Importer* importer;
	const aiScene* scene; // assimp는 fbx처럼 접두어로 ai가 붙음

	wstring fbxFile;
	wstring saveFolder;
	wstring saveName;

	vector<struct AsMaterial *> materials;
	vector<struct AsBone*> bones;
	vector<struct AsMesh*> meshes;
};