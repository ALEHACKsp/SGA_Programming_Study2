#include "stdafx.h"
#include "FbxLoader.h"
#include "FbxType.h"
#include "FbxUtility.h"
#include "Utilities/Xml.h"
#include "Utilities/BinaryFile.h"

FbxLoader::FbxLoader(wstring file, wstring saveFolder, wstring saveName)
	: fbxFile(file), saveFolder(saveFolder), saveName(saveName)
{
	manager = FbxManager::Create();
	scene = FbxScene::Create(manager, ""); // 2번째 인자 씬 이름

	ios = FbxIOSettings::Create(manager, IOSROOT);
	ios->SetBoolProp(IMP_FBX_TEXTURE, true);
	manager->SetIOSettings(ios);

	importer = FbxImporter::Create(manager, "");

	string sFile = String::ToString(file);
	bool b = importer->Initialize(sFile.c_str(), -1, ios);
	// 파일 부를 수 없으면 false 넘어옴
	assert(b == true);

	// 열어서 scene정보를 scene에 import 하라는 거
	b = importer->Import(scene);
	assert(b == true);

	FbxUtility::Axis = scene->GetGlobalSettings().GetAxisSystem();

	FbxSystemUnit unit = scene->GetGlobalSettings().GetSystemUnit();
	// 미터로 맞춰 주려고 하는거 크기 일정하게 하기 위해
	if (unit != FbxSystemUnit::m)
	{
		FbxSystemUnit::ConversionOptions option = 
		{
			false, // 부모노드로부터 단위 상속 받을껀지
			true, // 제안선 크기도 변환 할 건지 이미 크기 정해둠
			true, // cluster 하나하나가 관절 사이인데 이것도 변환할 건지
			true, // normal 값도 변경할건지
			true, // 측광 라이트 속성도 변경할건지 (tangent 의미)
			true // fbx camera 안쓰면 상관없는데 이것도 변경할 꺼
		};

		FbxSystemUnit::m.ConvertScene(scene, option);
	}

	FbxGeometryConverter gc(manager);
	// 이게 좀 느림
	gc.Triangulate(scene, true);
}

FbxLoader::~FbxLoader()
{
	ios->Destroy();
	importer->Destroy();
	scene->Destroy();
	manager->Destroy();
}

void FbxLoader::ExportMaterial(wstring saveFolder, wstring fileName)
{
	ReadMaterial();

	wstring tempFolder = saveFolder.length() < 1 ? this->saveFolder : saveFolder;
	wstring tempName = fileName.length() < 1 ? this->saveName : fileName;

	WriteMaterial(tempFolder, tempName);
}

void FbxLoader::ExportMesh(wstring saveFolder, wstring fileName)
{
	ReadBoneData(scene->GetRootNode(), -1, -1);
	ReadSkinData();

	wstring tempFolder = saveFolder.length() < 1 ? this->saveFolder : saveFolder;
	wstring tempName = fileName.length() < 1 ? this->saveName : fileName;

	WriteMeshData(saveFolder, tempName);
}

void FbxLoader::GetClipList(vector<wstring>* list)
{
	list->clear();
	for (int i = 0; i < importer->GetAnimStackCount(); i++)
	{
		FbxTakeInfo* takeInfo = importer->GetTakeInfo(i);


		string name = takeInfo->mName.Buffer();
		list->push_back(String::ToWString(name));
	}
}

void FbxLoader::ExportAnimation(UINT clipNumber, wstring saveFloder, wstring fileName)
{
	FbxClip* clip = ReadAnimationData(clipNumber);

	wstring tempFolder = saveFolder.length() < 1 ? this->saveFolder : saveFolder;
	wstring tempName = fileName.length() < 1 ? this->saveName : fileName;

	WriteClipData(clip, tempFolder, tempName);
}

void FbxLoader::ExportAnimation(wstring clipName, wstring saveFloder, wstring fileName)
{
	FbxClip* clip = ReadAnimationData(clipName);

	wstring tempFolder = saveFolder.length() < 1 ? this->saveFolder : saveFolder;
	wstring tempName = fileName.length() < 1 ? this->saveName : fileName;

	WriteClipData(clip, tempFolder, tempName);
}

void FbxLoader::ReadMaterial()
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
			material->Diffuse = FbxUtility::ToColor
			(lambert->Diffuse, lambert->DiffuseFactor);
		}

		if (fbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId) == true)
		{
			FbxSurfacePhong* phong = (FbxSurfacePhong *)fbxMaterial;

			// 조명에는 투명도 안들어가서 alpha 값 강도로 씀
			material->Specular = FbxUtility::ToColor(phong->Specular, phong->SpecularFactor);
			material->SpecularExp = (float)phong->Shininess;
		}

		// 텍스처 꺼내오기
		FbxProperty prop;

		// 변수앞에 s는 문자열이란 뜻 e는 enum
		// 텍스처 파일 fbx 안에 압축되어있으면 주소 받아오는거
		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
		material->DiffuseFile = FbxUtility::GetTextureFile(prop);

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
		material->SpecularFile = FbxUtility::GetTextureFile(prop);

		prop = fbxMaterial->FindProperty(FbxSurfaceMaterial::sNormalMap);
		material->NormalFile = FbxUtility::GetTextureFile(prop);

		materials.push_back(material);
	}
}

void FbxLoader::WriteMaterial(wstring saveFolder, wstring fileName)
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
		WriteXmlColor(document, element, material->Diffuse);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		WriteXmlColor(document, element, material->Specular);
		node->LinkEndChild(element);

		element = document->NewElement("SpecularExp");
		element->SetText(material->SpecularExp);
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		CopyTextureFile(material->DiffuseFile, saveFolder);
		element->SetText(material->DiffuseFile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		CopyTextureFile(material->SpecularFile, saveFolder);
		element->SetText(material->SpecularFile.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		CopyTextureFile(material->NormalFile, saveFolder);
		element->SetText(material->NormalFile.c_str());
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

void FbxLoader::ReadBoneData(FbxNode* node, int index, int parent)
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

			bone->LocalTransform = FbxUtility::ToMatrix(
				node->EvaluateLocalTransform());
			bone->GlobalTransform = FbxUtility::ToMatrix(
				node->EvaluateGlobalTransform());
			boneDatas.push_back(bone);

			if (nodeType == FbxNodeAttribute::eMesh)
				ReadMeshData(node, index);
			
		} // if(b)
	}

	// 트리구조라 재귀함수로 노드들 접근할꺼 
	for (int i = 0; i < node->GetChildCount(); i++)
		ReadBoneData(node->GetChild(i), boneDatas.size(), index);
}

UINT FbxLoader::GetBoneIndexByName(string name)
{
	for (UINT i = 0; i < boneDatas.size(); i++)
	{
		if (boneDatas[i]->Name == name)
			return i;
	}

	return -1;
}

void FbxLoader::ReadMeshData(FbxNode * node, int parentBone)
{
	// node attribute가 eMesh가 아니면 null값 반환
	FbxMesh* mesh = node->GetMesh();

	for (int i = 0; i < mesh->GetControlPointsCount(); i++)
	{
		FbxControlPointData cp;
		cp.Position = mesh->GetControlPointAt(i);
		cpDatas[i] = cp;
	}

	// Skin 부르는곳
	int deformerCount = mesh->GetDeformerCount(); // 위치가 변경되기 위한 컨트롤 포인트
												  // 이렇게 해도 됨
												  //vector<FbxBoneWeights> boneWeights(mesh->GetControlPointsCount(), FbxBoneWeights());

	for (int i = 0; i < deformerCount; i++)
	{
		FbxDeformer* deformer = mesh->GetDeformer(i, FbxDeformer::eSkin);

		FbxSkin* skin = reinterpret_cast<FbxSkin *> (deformer);
		if (skin == NULL) continue;

		for (int clusterIndex = 0; clusterIndex < skin->GetClusterCount(); clusterIndex++)
		{
			FbxCluster* cluster = skin->GetCluster(clusterIndex);
			assert(cluster->GetLinkMode() == FbxCluster::eNormalize);

			string linkName = cluster->GetLink()->GetName();
			UINT boneIndex = GetBoneIndexByName(linkName);

			FbxAMatrix transform;
			FbxAMatrix linkTransform;

			cluster->GetTransformMatrix(transform);
			cluster->GetTransformLinkMatrix(linkTransform);

			boneDatas[boneIndex]->LocalTransform = FbxUtility::ToMatrix(transform);
			boneDatas[boneIndex]->GlobalTransform = FbxUtility::ToMatrix(linkTransform.Inverse() * transform);

			int* indices = cluster->GetControlPointIndices();
			double* weights = cluster->GetControlPointWeights();

			// cluster 영향을 받을 본들의 연결고리
			for (int indexCount = 0; indexCount < cluster->GetControlPointIndicesCount(); indexCount++)
			{
				FbxVertexWeightData vertexWeight;
				vertexWeight.Index = boneIndex;
				vertexWeight.Weight = weights[indexCount];

				UINT cpIndex = indices[indexCount];
				cpDatas[cpIndex].Datas.push_back(vertexWeight);
			}
		} // for(cluster Index)
	} // for(deformer)

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
			int pvi = FbxUtility::Axis.GetCoorSystem() == FbxAxisSystem::eRightHanded ? 2 - vi : vi;
			// control point index
			int cpIndex = mesh->GetPolygonVertex(p, pvi);

			FbxVertex* vertex = new FbxVertex();
			vertex->ControlPoint = cpIndex;

			vertex->Vertex.Position = FbxUtility::ToPosition(cpDatas[cpIndex].Position);

			FbxVector4 normal;
			mesh->GetPolygonVertexNormal(p, vi, normal);
			vertex->Vertex.Normal = FbxUtility::ToNormal(normal);

			// 해당 위치에 있는 정점이 소속될 material의 이름을 가져오는 함수
			vertex->MaterialName = FbxUtility::GetMaterialName(mesh, p, cpIndex);

			int uvIndex = mesh->GetTextureUVIndex(p, vi);
			vertex->Vertex.Uv = FbxUtility::GetUv(mesh, cpIndex, uvIndex);

			vertex->Vertex.BlendIndices = FbxUtility::ToVertexIndices(cpDatas[cpIndex].Datas);
			vertex->Vertex.BlendWeights = FbxUtility::ToVertexWeights(cpDatas[cpIndex].Datas);

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

void FbxLoader::ReadSkinData()
{
	for (FbxMeshData* meshData : meshDatas)
	{
		FbxMesh* mesh = meshData->Mesh;

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
				VertexTextureNormalTangentBlend vertex;
				vertex = temp->Vertex;

				meshPart->Vertices.push_back(vertex);
				// 추후 index 최적화해서 넣을거 일단은 순서대로 넣는식으로 구현
				meshPart->Indices.push_back(meshPart->Indices.size());
			}
			meshData->MeshParts.push_back(meshPart);
		}
	}
}

void FbxLoader::WriteMeshData(wstring saveFolder, wstring fileName)
{
	Path::CreateFolders(saveFolder);

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
			w->Byte(&part->Vertices[0], sizeof(VertexTextureNormalTangentBlend) * part->Vertices.size());

			w->UInt(part->Indices.size());
			w->Byte(&part->Indices[0], sizeof(UINT) * part->Indices.size());

			SAFE_DELETE(part);
		}

		SAFE_DELETE(meshData);
	}

	w->Close();
	SAFE_DELETE(w);
}

void FbxLoader::WriteXmlColor(Xml::XMLDocument* document, Xml::XMLElement * element, D3DXCOLOR & color)
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

void FbxLoader::CopyTextureFile(OUT string & textureFile, wstring & saveFolder)
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

FbxClip* FbxLoader::ReadAnimationData(wstring name)
{
	UINT number = -1; // 최대값됨
	for (UINT i = 0; i < (UINT)importer->GetAnimStackCount(); i++)
	{
		FbxTakeInfo* takeInfo = importer->GetTakeInfo(i);
		wstring compare = String::ToWString(takeInfo->mName.Buffer());

		if (name == compare)
		{
			number = i;
			break;
		}
	}

	if (number != -1)
		return ReadAnimationData(number);

	return NULL;
}

FbxClip* FbxLoader::ReadAnimationData(UINT index)
{
	UINT stackCount = (UINT)importer->GetAnimStackCount();
	assert(index < stackCount);

	// 애니메이션 다 씬에 붙어있음
	FbxTime::EMode mode = scene->GetGlobalSettings().GetTimeMode(); // 씬에 셋팅 정보 다 넘어봄 이중 타임노드만
	float frameRate = (float)FbxTime::GetFrameRate(mode);

	// 이녀석에서 legacy인지 아닌지 갈림
	// mesh에 대한 animation 1:1인게 legacy

	FbxClip* clip = new FbxClip();
	clip->FrameRate = frameRate;

	FbxTakeInfo* takeInfo = importer->GetTakeInfo(index);
	clip->Name = takeInfo->mName.Buffer();

	// 시간 간격을 관리해주는 애
	FbxTimeSpan span = takeInfo->mLocalTimeSpan;
	FbxTime tempDuration = span.GetDuration();

	int start = (int)span.GetStart().GetFrameCount(); // 항상 0이 아닐 수 있음
	int stop = (int)span.GetStop().GetFrameCount();
	float duration = (float)tempDuration.GetMilliSeconds();

	if (start < stop)
		ReadKeyframeData(clip, scene->GetRootNode(), start, stop);

	clip->Duration = duration;
	clip->FrameCount = (stop - start) + 1; // 0부터 시작이라

	return clip;
}

void FbxLoader::ReadKeyframeData(FbxClip * clip, FbxNode * node, int start, int stop)
{
	FbxNodeAttribute* attribute = node->GetNodeAttribute();

	if (attribute != NULL)
	{
		FbxNodeAttribute::EType nodeType = attribute->GetAttributeType();

		// 요놈이 애니메이션될 본
		if (nodeType == FbxNodeAttribute::eSkeleton)
		{
			FbxKeyframe* keyframe = new FbxKeyframe();
			keyframe->BoneName = node->GetName();

			// 원래는 이벤트가 발생할때마다 저장인데 보기 편하게 매프레임마다 저장할꺼
			for (int i = start; i <= stop; i++)
			{
				FbxTime animationTime;
				animationTime.SetFrame(i);

				// affine 변환된 매트릭스
				FbxAMatrix matrix = node->EvaluateLocalTransform(animationTime);
				D3DXMATRIX transform = FbxUtility::ToMatrix(matrix);

				FbxKeyframeData data;
				data.Time = (float)animationTime.GetMilliSeconds();
				D3DXMatrixDecompose(&data.Scale, &data.Rotation, &data.Translation, &transform);

				keyframe->Transforms.push_back(data);
			}

			clip->Keyframes.push_back(keyframe);
		} // if(nodeType)
	}

	for (int i = 0; i < node->GetChildCount(); i++)
		ReadKeyframeData(clip, node->GetChild(i), start, stop);
}

void FbxLoader::WriteClipData(FbxClip * clip, wstring saveFolder, wstring fileName)
{
	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + fileName);

	w->String(clip->Name);
	w->Float(clip->Duration);
	w->Float(clip->FrameRate);
	w->UInt(clip->FrameCount);

	w->UInt(clip->Keyframes.size());
	for (FbxKeyframe* keyframe : clip->Keyframes)
	{
		w->String(keyframe->BoneName);

		w->UInt(keyframe->Transforms.size());
		w->Byte(&keyframe->Transforms[0], sizeof(FbxKeyframeData) * keyframe->Transforms.size());

		SAFE_DELETE(keyframe);
	}

	w->Close();
	SAFE_DELETE(w);
}
