#include "stdafx.h"
#include "Exporter.h"
#include "Type.h"
#include "Utility.h"

#include "Utilities/Xml.h"
#include "Utilities/BinaryFile.h"

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

void Fbx::Exporter::ExportMesh(wstring saveFolder, wstring fileName)
{
	ReadBoneData(scene->GetRootNode(), -1, -1);
	ReadSkinData();

	WriteMeshData(saveFolder, fileName);
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

			// 조명에는 투명도 안들어가서 alpha 값 강도로 씀
			material->Diffuse = Utility::ToColor
				(lambert->Diffuse, lambert->DiffuseFactor);
		}

		// 텍스처 꺼내오기
		FbxProperty prop;

		// 변수앞에 s는 문자열이란 뜻 e는 enum
		// 텍스처 파일 fbx 안에 압축되어있으면 주소 받아오는거
		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		material->DiffuseFile = Utility::GetTextureFile(prop);

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
		root->LinkEndChild(node); // 뒤에 붙이려고 하는거

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

		SAFE_DELETE(material);
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

void Fbx::Exporter::ReadBoneData(FbxNode* node, int index, int parent)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute != NULL) 
	{
		// TODO: 본 로딩 처리

		// boneDatas.push_back(data); // 이런식으로 할 꺼

		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();
		
		bool b = false;
		b |= (nodeType == FbxNodeAttribute::eSkeleton);
		b |= (nodeType == FbxNodeAttribute::eMesh);
		b |= (nodeType == FbxNodeAttribute::eNull);
		b |= (nodeType == FbxNodeAttribute::eMarker);

		if (b == true)
		{
			FbxBoneData* bone = new FbxBoneData();
			bone->Index = index;
			bone->Parent = parent;
			bone->Name = node->GetName();
			bone->LocalTransform = Utility::ToMatrix(
				node->EvaluateLocalTransform());
			bone->GlobalTransform = Utility::ToMatrix(
				node->EvaluateGlobalTransform());
			boneDatas.push_back(bone);
			
			if (nodeType == FbxNodeAttribute::eMesh) 
			{
				// TODO: 메시 데이터 로딩
				// scene에다 할 수있는데 오래걸리므로 필요한거만 convert
				// 2번째 true 바꿔서 다시 넣어주는거 false 반환값
				// 3번째 false 요즘 방식으로 하는건데 복잡해서 구형 방식으로 true
				converter->Triangulate(attribute, true, true);

				ReadMeshData(node, index);
			}
		} // if(b)
	}

	// 트리구조라 재귀함수로 노드들 접근할꺼 
	for (int i = 0; i < node->GetChildCount(); i++)
		ReadBoneData(node->GetChild(i), boneDatas.size(), index);
}

void Fbx::Exporter::ReadMeshData(FbxNode * node, int parentBone)
{
	// node attribute가 eMesh가 아니면 null값 반환
	FbxMesh* mesh = node->GetMesh();

	vector<FbxVertex *> vertices;
	for (int p = 0; p < mesh->GetPolygonCount(); p++)
	{
		// 이 삼각형의 꼭지점의 갯수가 나옴
		// 3개가 아닐 수도 있음
		int vertexInPolygon = mesh->GetPolygonSize(p);
		assert(vertexInPolygon == 3);

		// 2 1 0 순으로 할꺼
		// 왼손좌표계라서 시계방향으로 가야되는데 
		// fbx는 반시계로 되어있어서 2 1 0 순으로 하는거
		for (int vi = vertexInPolygon - 1; vi >= 0; vi--)
		{
			FbxVertex* vertex = new FbxVertex();

			// control point index
			int cpIndex = mesh->GetPolygonVertex(p, vi);
			vertex->ControlPoint = cpIndex;


			D3DXVECTOR3 temp;

			FbxVector4 position = mesh->GetControlPointAt(cpIndex);
			temp = Utility::ToVector3(position);
			// Negative - fbx 오른손 좌표계를 왼손좌표계로 돌려주는 함수
			// 뒤집는 부분에 대해서는 나중에 설명해주실꺼
			D3DXVec3TransformCoord(&vertex->Vertex.Position, &temp, &Utility::Negative());

			FbxVector4 normal;
			mesh->GetPolygonVertexNormal(p, vi, normal);
			normal.Normalize();
			temp = Utility::ToVector3(normal);
			// TrasnformCoord가 아니라 TransformNormal로 해야되는데
			// TrasnformCoord로 해도 이동변환이 없어서 노멀이랑 같아서 Coord로 한거
			D3DXVec3TransformCoord(&vertex->Vertex.Normal, &temp, &Utility::Negative());


			// 해당 위치에 있는 정점이 소속될 material의 이름을 가져오는 함수
			vertex->MaterialName = Utility::GetMaterialName(mesh, p, cpIndex);

			int uvIndex = mesh->GetTextureUVIndex(p, vi);
			vertex->Vertex.Uv = Utility::GetUv(mesh, cpIndex, uvIndex);

			vertices.push_back(vertex);
		} // for(vi)


		UINT index0 = vertices.size() - 3;
		UINT index1 = vertices.size() - 2;
		UINT index2 = vertices.size() - 1;

		D3DXVECTOR3 p0 = vertices[index0]->Vertex.Position;
		D3DXVECTOR3 p1 = vertices[index1]->Vertex.Position;
		D3DXVECTOR3 p2 = vertices[index2]->Vertex.Position;

		D3DXVECTOR3 uv0 = vertices[index0]->Vertex.Uv;
		D3DXVECTOR3 uv1 = vertices[index1]->Vertex.Uv;
		D3DXVECTOR3 uv2 = vertices[index2]->Vertex.Uv;

		
		// 정규 직교
		D3DXVECTOR3 e0 = p1 - p0;
		D3DXVECTOR3 e1 = p2 - p0;

		float u0 = uv1.x - uv0.x;
		float u1 = uv2.x - uv0.x;
		float v0 = uv1.y - uv0.y;
		float v1 = uv2.y - uv0.y;
		float r = 1.0f / (u0 * v1 - v0 * u1);

		// tangent 벡터
		D3DXVECTOR3 tangent;
		tangent.x = r * (v1 * e0.x - v0 * e1.x);
		tangent.y = r * (v1 * e0.y - v0 * e1.y);
		tangent.z = r * (v1 * e0.z - v0 * e1.z);

		vertices[index0]->Vertex.Tangent += tangent;
		vertices[index1]->Vertex.Tangent += tangent;
		vertices[index2]->Vertex.Tangent += tangent;
	} // for(p)

	for (UINT i = 0; i < vertices.size(); i++)
	{
		D3DXVECTOR3 n = vertices[i]->Vertex.Normal;
		D3DXVECTOR3 t = vertices[i]->Vertex.Tangent;

		D3DXVECTOR3 temp = (t - n * D3DXVec3Dot(&n, &t));
		D3DXVec3Normalize(&temp, &temp);

		vertices[i]->Vertex.Tangent = temp;
		// binomal은 tangent와 normal의 외적으로 구함
	}

	FbxMeshData* meshData = new FbxMeshData();
	meshData->Name = node->GetName();
	meshData->ParentBone = parentBone;
	meshData->Vertices = vertices;
	meshData->Mesh = mesh;
	meshDatas.push_back(meshData);

}

void Fbx::Exporter::ReadSkinData()
{
	for (FbxMeshData* meshData : meshDatas)
	{
		// mesh part 별로 분할을 해줘야함
		for (int i = 0; i < scene->GetMaterialCount(); i++)
		{
			FbxSurfaceMaterial* material = scene->GetMaterial(i);
			string materialName = material->GetName();

			vector<FbxVertex *> gather;
			for (FbxVertex* temp : meshData->Vertices)
			{
				if (temp->MaterialName == materialName)
					gather.push_back(temp);
			}
			if (gather.size() < 1) continue;

			FbxMeshPartData* meshPart = new FbxMeshPartData();
			meshPart->MaterialName = materialName;

			for (FbxVertex* temp : gather)
			{
				ModelVertexType vertex;
				vertex = temp->Vertex;

				meshPart->Vertices.push_back(vertex);
				// 추후 index 최적화해서 넣을거 일단은 순서대로 넣는식으로 구현
				meshPart->Indices.push_back(meshPart->Indices.size());
			}
			meshData->MeshParts.push_back(meshPart);
		}
	}
}

void Fbx::Exporter::WriteMeshData(wstring saveFolder, wstring fileName)
{
	Path::CreateFolder(saveFolder);

	BinaryWriter* w = new BinaryWriter();
	// 저장할 파일 열기
	w->Open(saveFolder + fileName);

	// Bond Data 저장
	w->UInt(boneDatas.size());
	for (FbxBoneData* bone : boneDatas)
	{
		w->Int(bone->Index);
		w->String(bone->Name);
		w->Int(bone->Parent);

		w->Matrix(bone->LocalTransform);
		w->Matrix(bone->GlobalTransform);

		SAFE_DELETE(bone);
	}

	// Mesh Data
	w->UInt(meshDatas.size());
	for (FbxMeshData* meshData : meshDatas)
	{
		w->String(meshData->Name);
		w->Int(meshData->ParentBone);

		w->UInt(meshData->MeshParts.size());
		for (FbxMeshPartData* part : meshData->MeshParts)
		{
			w->String(part->MaterialName);

			w->UInt(part->Vertices.size());
			w->Byte(&part->Vertices[0], sizeof(ModelVertexType) * part->Vertices.size());

			w->UInt(part->Indices.size());
			w->Byte(&part->Indices[0], sizeof(UINT) * part->Indices.size());

			SAFE_DELETE(part);
		}

		SAFE_DELETE(meshData);
	}

	w->Close();
	SAFE_DELETE(w);
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
		// 1번째 인자 복사할 파일 경로, 2번째 인자 새로운 파일 경로, 
		// false 덮어쓰기, true면 같은 파일 있으면 fail
		CopyFile(file.c_str(), (saveFolder + fileName).c_str(), FALSE);

	textureFile = String::ToString(fileName);
}
