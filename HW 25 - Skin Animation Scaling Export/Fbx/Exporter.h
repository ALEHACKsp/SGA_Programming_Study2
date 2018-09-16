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
		void ExportMesh(wstring saveFolder, wstring fileName, 
			D3DXVECTOR3 scale = D3DXVECTOR3(1.0f,1.0f,1.0f));
		
		void GetClipList(vector<wstring>* list);
		void ExportAnimation(wstring saveFloder, wstring fileName, UINT clipNumber, D3DXVECTOR3 scale = D3DXVECTOR3(1,1,1));
		void ExportAnimation(wstring saveFloder, wstring fileName, wstring clipName, D3DXVECTOR3 scale = D3DXVECTOR3(1, 1, 1));

	private:
		void ReadMaterial();
		void WriteMaterial(wstring saveFolder, wstring fileName);

		void ReadBoneData(FbxNode* node, int index, int parent, D3DXVECTOR3 scale = D3DXVECTOR3(1,1,1));

		UINT GetBoneIndexByName(string name);
		void ReadMeshData(FbxNode* node, int parentBone, D3DXVECTOR3 scale);
		void ReadSkinData(D3DXVECTOR3 scale);
		void WriteMeshData(wstring saveFolder, wstring fileName);

		struct FbxClip* ReadAnimationData(UINT index, D3DXVECTOR3 scale);
		struct FbxClip* ReadAnimationData(wstring name, D3DXVECTOR3 scale);
		void ReadKeyframeData(Fbx::FbxClip * clip, FbxNode * node, int start, int stop, D3DXVECTOR3 scale);

		void WriteClipData(struct FbxClip* clip, wstring saveFolder, wstring fileName);

		void Scaling(D3DXVECTOR3 scale);

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

