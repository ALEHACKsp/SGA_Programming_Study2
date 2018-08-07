#pragma once

// fbx������ ���ϴ� ���·� �����ϴ� Ŭ����

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

