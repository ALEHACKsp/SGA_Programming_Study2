#pragma once

// fbx파일을 원하는 형태로 추출하는 클래스

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

		// fbx에서 재질 정보 정리해서 저장하는 함수
		void ExportMaterial(wstring saveFolder, wstring fileName);
		void ExportMesh(wstring saveFolder, wstring fileName);

	private:
		void ReadMaterial();
		void WriteMaterial(wstring saveFolder, wstring fileName);

		void ReadBoneData(FbxNode* node, int index, int parent);
		void ReadMeshData(FbxNode* node, int parentBone);
		void ReadSkinData();
		void WriteMeshData(wstring saveFolder, wstring fileName);

	private:
		FbxManager* manager;
		FbxImporter* importer;
		FbxScene* scene;
		FbxIOSettings* ios;
		FbxGeometryConverter* converter;

	private:
		void WriteXmlColor(Xml::XMLDocument* document,
			Xml::XMLElement* element, D3DXCOLOR& color);

		// OUT 의미는 없는데 리턴될 변수라는 마킹만 해주는거
		void CopyTextureFile(OUT string& textureFile, wstring& saveFolder);

	private:
		vector<struct FbxMaterial *> materials;

		vector<struct FbxBoneData *> boneDatas;
		vector<struct FbxMeshData *> meshDatas;
	};
}

