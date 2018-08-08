#include "stdafx.h"
#include "Exporter.h"
#include "Type.h"
#include "Utility.h"
#include "../Utilities/Xml.h"

Fbx::Exporter::Exporter(wstring file)
{
	manager = FbxManager::Create();
	scene = FbxScene::Create(manager, ""); // 2번째 인자 씬 이름

	ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	manager->SetIOSettings(ios);

	importer = FbxImporter::Create(manager, "");

	string sFile = String::ToString(file);
	bool b = importer->Initialize(sFile.c_str(), -1, ios);
	// 파일 불를 수 없으면 false 넘어옴
	assert(b == true);

	// 열어서 scene정보를 scene에 import 하라는 거
	b = importer->Import(scene);
	assert(b == true);

	converter = new FbxGeometryConverter(manager);
}

Fbx::Exporter::~Exporter()
{
	SAFE_DELETE(converter);

	ios->Destroy();
	importer->Destroy();
	scene->Destroy();
	manager->Destroy();
}

void Fbx::Exporter::ExportMaterial(wstring saveFolder, wstring fileName)
{
	ReadMaterial();
	WriteMaterial(saveFolder, fileName);
}

void Fbx::Exporter::ReadMaterial()
{
	int count = scene->GetMaterialCount();
	
	for (int i = 0; i < count; i++)
	{
		FbxSurfaceMaterial* fbxMaterial = scene->GetMaterial(i);
		
		FbxMaterial * material = new FbxMaterial();
		// 이름이 중요함 나중에 mesh에서 선택할 때 이름 기준으로 하게됨
		material->Name = fbxMaterial->GetName();

		// down casting 가능한지 확인하는 조건문
		// FbxMaterial이 상위 클래스 FbxSurfaceLambert 하위 클래스
		if (fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId) == true)
		{
			FbxSurfaceLambert* lambert = (FbxSurfaceLambert *)fbxMaterial;

			material->Diffuse = Utility::ToColor
				(lambert->Diffuse, lambert->DiffuseFactor);
		}

		// 텍스처 꺼내오기
		FbxProperty prop;

		// 변수앞에 s는 문자열이란 뜻 e는 enum
		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		material->DiffuseFile = Utility::GetTextrueFile(prop);

		materials.push_back(material);
	}
}

void Fbx::Exporter::WriteMaterial(wstring saveFolder, wstring fileName)
{
	// 하위까지 폴더까지 경로 검사하면서 만들어주는 함수
	Path::CreateFolders(saveFolder);

	Xml::XMLDocument * document = new Xml::XMLDocument();
	Xml::XMLDeclaration *decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Materials");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	// 버전 맞춰야되서 위에 부분 추가한거

	for (FbxMaterial * material : materials)
	{
		Xml::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		Xml::XMLElement * element = NULL;

		element = document->NewElement("Name");
		element->SetText(material->Name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		node->LinkEndChild(element);

		WriteXmlColor(document, element, material->Diffuse);

		element = document->NewElement("DiffuseFile");
		CopyTextureFile(material->DiffuseFile, saveFolder);
		element->SetText(material->DiffuseFile.c_str());
		node->LinkEndChild(element);
	}

	//Xml::XMLElement* temp1 = document->NewElement("Temp1");
	//temp1->SetText(10.5f);
	//root->LinkEndChild(temp1); // 뒤에 붙이려고 하는거
	//
	//Xml::XMLElement* temp2 = document->NewElement("Temp2");
	//temp2->SetText(20);
	//root->LinkEndChild(temp2);

	string file = String::ToString(saveFolder + fileName);
	document->SaveFile(file.c_str());
}

void Fbx::Exporter::WriteXmlColor(Xml::XMLDocument* document, Xml::XMLElement * element, D3DXCOLOR & color)
{
	Xml::XMLElement * r = document->NewElement("R");
	r->SetText(color.r);
	element->LinkEndChild(r);

	Xml::XMLElement * g = document->NewElement("G");
	g->SetText(color.g);
	element->LinkEndChild(g);

	Xml::XMLElement * b = document->NewElement("B");
	b->SetText(color.b);
	element->LinkEndChild(b);

	Xml::XMLElement * a = document->NewElement("A");
	a->SetText(color.a);
	element->LinkEndChild(a);
}

void Fbx::Exporter::CopyTextureFile(OUT string & textureFile, wstring & saveFolder)
{
	if (textureFile.length() < 1)
		return;

	wstring file = String::ToWString(textureFile);
	// GetFileName 파일명만 빼오는 함수 (경로에서)
	wstring fileName = Path::GetFileName(file);

	if (Path::ExistFile(textureFile) == true)
		// api 함수
		// 마지막 인자는 성공여부 체크
		CopyFile(file.c_str(), (saveFolder + fileName).c_str(), FALSE);

	textureFile = String::ToString(fileName);
}
