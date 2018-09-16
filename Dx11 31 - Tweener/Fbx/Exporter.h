#pragma once

// fbx������ ���ϴ� ���·� �����ϴ� Ŭ����

namespace Xml
{
	class XMLDocument;
	class XMLElement;
}

namespace Fbx 
{
	class Exporter
	{
	public:
		Exporter(wstring file);
		~Exporter();

		// fbx���� ���� ���� �����ؼ� �����ϴ� �Լ�
		void ExportMaterial(wstring saveFolder, wstring fileName);
		void ExportMesh(wstring saveFolder, wstring fileName);
		
		void GetClipList(vector<wstring>* list);
		void ExportAnimation(wstring saveFloder, wstring fileName, UINT clipNumber);
		void ExportAnimation(wstring saveFloder, wstring fileName, wstring clipName);

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
		void ReadKeyframeData(Fbx::FbxClip * clip, FbxNode * node, int start, int stop);

		void WriteClipData(struct FbxClip* clip, wstring saveFolder, wstring fileName);

	private:
		FbxManager* manager;
		FbxImporter* importer;
		FbxScene* scene;
		FbxIOSettings* ios;
		FbxGeometryConverter* converter;

	private:
		void WriteXmlColor(Xml::XMLDocument* document,
			Xml::XMLElement* element, D3DXCOLOR& color);

		// OUT �ǹ̴� ���µ� ���ϵ� ������� ��ŷ�� ���ִ°�
		void CopyTextureFile(OUT string& textureFile, wstring& saveFolder);

	private:
		vector<struct FbxMaterial *> materials;

		vector<struct FbxBoneData *> boneDatas;
		vector<struct FbxMeshData *> meshDatas;
	};
}

