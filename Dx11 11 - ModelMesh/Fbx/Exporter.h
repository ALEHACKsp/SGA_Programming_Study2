#pragma once

// fbx파일을 원하는 형태로 추출하는 클래스

namespace Fbx 
{
	class Exporter
	{
	public:
		Exporter(wstring file);
		~Exporter();

	private:
		FbxManager* manager;
		FbxImporter* importer;
		FbxScene* scene;
		FbxIOSettings* ios;
		FbxGeometryConverter* converter;
	};
}

