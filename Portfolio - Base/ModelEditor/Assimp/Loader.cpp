#include "stdafx.h"
#include "Loader.h"
#include "Type.h"

#include "Utilities/Xml.h"
#include "Utilities/BinaryFile.h"

Loader::Loader(wstring file, wstring saveFolder, wstring saveName)
	: fbxFile(file), saveFolder(saveFolder), saveName(saveName)
{
	importer = new Assimp::Importer();

	scene = NULL;
	scene = importer->ReadFile(String::ToString(fbxFile),
		aiProcess_ConvertToLeftHanded | aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
	assert(scene != NULL);
}

Loader::~Loader()
{
	SAFE_DELETE(importer); // importer 날리면 scene까지 날라간다고함
}

void Loader::ExportMaterial(wstring saveFolder, wstring fileName)
{
	ReadMaterial();

	wstring tempFolder = saveFolder.length() < 1 ? this->saveFolder : saveFolder;
	wstring tempName = fileName.length() < 1 ? this->saveName : fileName;

	WriteMaterial(tempFolder, tempName + L".material");
}

void Loader::ExportMesh(wstring saveFolder, wstring fileName)
{
	ReadBoneData(scene->mRootNode, -1, -1);
	ReadSkinData();

	wstring tempFolder = saveFolder.length() < 1 ? this->saveFolder : saveFolder;
	wstring tempName = fileName.length() < 1 ? this->saveName : fileName;

	WriteMeshData(tempFolder, tempName + L".mesh");
}

void Loader::ExportAnimation(wstring saveFloder, wstring fileName)
{
	// assert false인 경우 프로그램 종료
	assert(scene->HasAnimations());

	wstring tempFolder = saveFolder.length() < 1 ? this->saveFolder : saveFolder;
	wstring tempName = fileName.length() < 1 ? this->saveName : fileName;

	for (UINT i = 0; i < scene->mNumAnimations; i++)
	{
		AsClip* clip = ReadAnimationData(i);

		WriteClipData(clip, tempFolder, tempName + L".animation");
	}
}

void Loader::ReadMaterial()
{
	UINT count = scene->mNumMaterials;

	for (UINT i = 0; i < count; i++)
	{
		aiMaterial* asMaterial = scene->mMaterials[i];

		AsMaterial* material = new AsMaterial;

		// string류는 이렇게 초기화하면 안됨
		//ZeroMemory(material, sizeof(AsMaterial));

		// aiString assimp에서 만든 string
		material->Name = asMaterial->GetName().C_Str();

		aiColor3D color;
		
		asMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->Diffuse = D3DXCOLOR(color.r, color.g, color.b, 1.0f);

		asMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material->Specular = D3DXCOLOR(color.r, color.g, color.b, 1.0f);

		float specular;
		asMaterial->Get(AI_MATKEY_SHININESS, specular);
		material->SpecularExp = specular;

		asMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, specular);
		material->Specular.a = specular;

		// 여기서 주의할꺼 있음
		aiString file;

		asMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		material->DiffuseFile = file.C_Str();

		asMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->SpecularFile = file.C_Str();

		asMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->NormalFile = file.C_Str();

		materials.push_back(material);
	}
}

void Loader::WriteMaterial(wstring saveFolder, wstring fileName)
{
	Path::CreateFolders(saveFolder);

	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLDeclaration *decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Materials");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	for (AsMaterial* material : materials)
	{
		Xml::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);


		Xml::XMLElement* element = NULL;

		element = document->NewElement("Name");
		element->SetText(material->Name.c_str());
		node->LinkEndChild(element);


		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(material->DiffuseFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(material->SpecularFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(material->NormalFile).c_str());
		node->LinkEndChild(element);


		element = document->NewElement("Diffuse");
		node->LinkEndChild(element);
		WriteXmlColor(document, element, material->Diffuse);

		element = document->NewElement("Specular");
		node->LinkEndChild(element);
		WriteXmlColor(document, element, material->Specular);

		element = document->NewElement("SpecularExp");
		if (material->SpecularExp == 0)
			element->SetText(1);
		else
			element->SetText(material->SpecularExp);
		node->LinkEndChild(element);

		SAFE_DELETE(material);
	}

	string file = String::ToString(saveFolder + fileName);
	document->SaveFile(file.c_str());

	SAFE_DELETE(document);
}

void Loader::ReadBoneData(aiNode * node, int index, int parent)
{
	AsBone* bone = new AsBone();
	bone->Index = index;
	bone->Parent = parent;
	bone->Name = node->mName.C_Str();

	// 생성자에 포인터로 받는게 있어서 그냥 이대로 넣음 된다고함
	// transform global이 아니라 local이라고 함 부모 자식 관계 계산해서 넣어줘야한다고함
	D3DXMATRIX transform(node->mTransformation[0]);
	D3DXMatrixTranspose(&bone->Transform, &transform);

	D3DXMATRIX temp;
	// 부모 없으면
	if (parent < 0)
		D3DXMatrixIdentity(&temp);
	else
		temp = bones[parent]->Transform;

	//D3DXMatrixInverse(&bone->Transform, NULL, &bone->Transform);
	//bone->Transform = temp * bone->Transform;
	bone->Transform = bone->Transform * temp;

	bones.push_back(bone);

	ReadMeshData(node, index);

	for (int i = 0; i < node->mNumChildren; i++)
		ReadBoneData(node->mChildren[i], bones.size(), index);
}

void Loader::ReadSkinData()
{
	for (UINT i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh* aiMesh = scene->mMeshes[i];

		if (!aiMesh->HasBones()) continue;

		AsMesh* mesh = meshes[i];

		vector<AsBoneWeights> boneWeights;
		boneWeights.assign(mesh->Vertices.size(), AsBoneWeights());

		for (UINT k = 0; k < aiMesh->mNumBones; k++)
		{
			aiBone*	aibone = aiMesh->mBones[k];
			UINT boneIndex = FindBoneIndex((string)aibone->mName.C_Str());

			for (UINT j = 0; j < aibone->mNumWeights; j++)
			{
				UINT index = aibone->mWeights[j].mVertexId;
				float weight = aibone->mWeights[j].mWeight;

				boneWeights[index].AddWeights(boneIndex, weight);
			}
		}

		for (UINT i = 0; i < boneWeights.size(); i++)
		{
			boneWeights[i].Normalize();

			AsBlendWeight blendWeight;
			boneWeights[i].GetBlendWeights(blendWeight);
			mesh->Vertices[i].BlendIndices = blendWeight.Indices;
			mesh->Vertices[i].BlendWeights = blendWeight.Weights;
		}
	}
}

void Loader::ReadMeshData(aiNode * node, int parentBone)
{
	if (node->mNumMeshes < 1) return;

	AsMesh* asMesh = new AsMesh();
	asMesh->Name = node->mName.C_Str();
	asMesh->ParentBone = parentBone;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		UINT index = node->mMeshes[i];
		aiMesh* mesh = scene->mMeshes[index];

		AsMeshPart* meshPart = new AsMeshPart();
		meshPart->Name = mesh->mName.C_Str();
		meshPart->MaterialName = scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();
		meshPart->StartVertex = asMesh->Vertices.size();
		meshPart->VertexCount = mesh->mNumVertices;
		meshPart->StartIndex = asMesh->Indices.size();
		meshPart->IndexCount = mesh->mNumFaces * mesh->mFaces->mNumIndices;

		for (UINT m = 0; m < mesh->mNumVertices; m++)
		{
			VertexTextureNormalTangentBlend vertex;
			memcpy(&vertex.Position, &mesh->mVertices[m], sizeof(D3DXVECTOR3));

			// fbx때도 layer 0꺼만 썼음
			if (mesh->HasTextureCoords(0) == true)
				memcpy(&vertex.Uv, &mesh->mTextureCoords[0][m], sizeof(D3DXVECTOR2));

			// has normal tangent도 있는데 aiprocess로 만들어준거
			// 그리고 없으면 null값 나와서 그렇게 체크해도 됨
			// biTangent까지 계산해줌 우리는 tangent만 받아서 계산했었음
			if (mesh->HasNormals())
				memcpy(&vertex.Normal, &mesh->mNormals[m], sizeof(D3DXVECTOR3));
			if (mesh->HasTangentsAndBitangents())
				memcpy(&vertex.Tangent, &mesh->mTangents[m], sizeof(D3DXVECTOR3));

			asMesh->Vertices.push_back(vertex);
		}

		for (UINT m = 0; m < mesh->mNumFaces; m++)
		{
			aiFace& face = mesh->mFaces[m];

			for (UINT f = 0; f < face.mNumIndices; f++)
			{
				asMesh->Indices.push_back(face.mIndices[f]);

				/*
				0 1 2 3 정점 4개면 사각형 그림
				4 5 6 7
				MeshPart MeshPart
				0 ~ 3, 4 ~ 7
				0,3    0, 3 (이렇게 들어와서 startVertex만큼 더해주는거)

				drawIndexed 할 때 baseVetex 0으로 하기위해서 더한다고 보면됨
				*/

				asMesh->Indices.back() += meshPart->StartVertex;
			}
		}

		asMesh->MeshParts.push_back(meshPart);
	} // for(i)

	meshes.push_back(asMesh);
}

void Loader::WriteMeshData(wstring saveFolder, wstring fileName)
{
	Path::CreateFolders(saveFolder);

	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + fileName);

	w->UInt(bones.size());
	for (AsBone* bone : bones)
	{
		w->Int(bone->Index);
		w->String(bone->Name);

		w->Int(bone->Parent);

		w->Matrix(bone->Transform);

		SAFE_DELETE(bone);
	}

	w->UInt(meshes.size());
	for (AsMesh* meshData : meshes)
	{
		w->String(meshData->Name);
		w->Int(meshData->ParentBone);

		w->UInt(meshData->Vertices.size());
		w->Byte(&meshData->Vertices[0], sizeof(VertexTextureNormalTangentBlend) * meshData->Vertices.size());

		w->UInt(meshData->Indices.size());
		w->Byte(&meshData->Indices[0], sizeof(UINT) * meshData->Indices.size());


		w->UInt(meshData->MeshParts.size());
		for (AsMeshPart* part : meshData->MeshParts)
		{
			w->String(part->Name);
			w->String(part->MaterialName);

			w->UInt(part->StartVertex);
			w->UInt(part->VertexCount);

			w->UInt(part->StartIndex);
			w->UInt(part->IndexCount);

			SAFE_DELETE(part);
		}

		SAFE_DELETE(meshData);
	}

	w->Close();
	SAFE_DELETE(w);
}

AsClip * Loader::ReadAnimationData(UINT index)
{
	aiAnimation* ani = scene->mAnimations[index];

	AsClip* clip = new AsClip();

	clip->FrameCount = ani->mDuration + 1;
	clip->FrameRate = ani->mTicksPerSecond;

	clip->Name = ani->mName.C_Str();

	vector<AsAniNode> aniNodeInfos;

	for (UINT i = 0; i < ani->mNumChannels; i++)
	{
		aiNodeAnim* aniNode = ani->mChannels[i];

		AsAniNode aniNodeInfo;
		aniNodeInfo.Name = aniNode->mNodeName;

		UINT keyCnt = max(aniNode->mNumPositionKeys, aniNode->mNumRotationKeys);
		keyCnt = max(keyCnt, aniNode->mNumScalingKeys);

		AsKeyframeData frameData;
		for (UINT k = 0; k < keyCnt; k++)
		{
			bool bFound = false;
			UINT t = aniNodeInfo.Keyframe.size();
			//if (aniNode->mNumPositionKeys > k) {
			if (fabsf(aniNode->mPositionKeys[k].mTime - t) <= D3DX_16F_EPSILON) {
				auto posKey = aniNode->mPositionKeys[k];
				memcpy_s(&frameData.Translation, sizeof(frameData.Translation), 
					&posKey.mValue, sizeof(posKey.mValue));
				frameData.Time = (float)aniNode->mPositionKeys[k].mTime;
				bFound = true;
			}

			//if (aniNode->mNumRotationKeys > k) {
			if (fabsf(aniNode->mRotationKeys[k].mTime - t) <= D3DX_16F_EPSILON) {
				auto rotKey = aniNode->mRotationKeys[k]; 
				frameData.Rotation.x = rotKey.mValue.x;
				frameData.Rotation.y = rotKey.mValue.y;
				frameData.Rotation.z = rotKey.mValue.z;
				frameData.Rotation.w = rotKey.mValue.w;
				frameData.Time = (float)aniNode->mRotationKeys[k].mTime;
				bFound = true;
			}

			//if (aniNode->mNumScalingKeys > k) {
			if (fabsf(aniNode->mScalingKeys[k].mTime - t) <= D3DX_16F_EPSILON) {
				auto scaleKey = aniNode->mScalingKeys[k];
				memcpy_s(&frameData.Scale, sizeof(frameData.Scale), &scaleKey.mValue, sizeof(scaleKey.mValue));
				frameData.Time = (float)aniNode->mScalingKeys[k].mTime;
				bFound = true;
			}
			
			if (bFound)
				aniNodeInfo.Keyframe.push_back(frameData);
		}

		if (aniNodeInfo.Keyframe.size() < clip->FrameCount) {
			AsKeyframeData keyframe = aniNodeInfo.Keyframe.back();
			UINT count = clip->FrameCount - aniNodeInfo.Keyframe.size();
			for (UINT i = 0; i < count; i++)
				aniNodeInfo.Keyframe.push_back(keyframe);
		}

		clip->Duration = max(clip->Duration, aniNodeInfo.Keyframe.back().Time);

		aniNodeInfos.push_back(aniNodeInfo); 
	}



	ReadKeyframeData(clip, scene->mRootNode, aniNodeInfos);

	return clip;
}

void Loader::WriteClipData(AsClip * clip, wstring saveFolder, wstring fileName)
{
	BinaryWriter* w = new BinaryWriter();
	w->Open(saveFolder + fileName);

	w->String(clip->Name);
	w->Float(clip->Duration);
	w->Float(clip->FrameRate);
	w->UInt(clip->FrameCount);

	w->UInt(clip->Keyframes.size());
	for (AsKeyframe* keyframe : clip->Keyframes)
	{
		w->String(keyframe->BoneName);

		w->UInt(keyframe->Transforms.size());
		w->Byte(&keyframe->Transforms[0], sizeof(AsKeyframeData) * keyframe->Transforms.size());

		SAFE_DELETE(keyframe);
	}

	w->Close();
	SAFE_DELETE(w);
}

void Loader::WriteXmlColor(Xml::XMLDocument * document, Xml::XMLElement * element, D3DXCOLOR & color)
{
	Xml::XMLElement* r = document->NewElement("R");
	r->SetText(color.r);
	element->LinkEndChild(r);

	Xml::XMLElement* g = document->NewElement("G");
	g->SetText(color.g);
	element->LinkEndChild(g);

	Xml::XMLElement* b = document->NewElement("B");
	b->SetText(color.b);
	element->LinkEndChild(b);

	Xml::XMLElement* a = document->NewElement("A");
	a->SetText(color.a);
	element->LinkEndChild(a);
}

// 바뀐 경로가 return 될 꺼
string Loader::WriteTexture(string file)
{
	if (file.length() < 1) return "";

	// 내장인지 아닌지 알아야 함

	// 내장이 아니면 null이 나옴
	// 이거말고 다른 방법있는데 (내장인지 아닌지) 이 방법이 가장 깔끔하다고 하심
	const aiTexture* texture = scene->GetEmbeddedTexture(file.c_str());
	if (texture != NULL) // 내장
	{
		string saveName = Path::GetFileNameWithoutExtension(file);
		saveName = String::ToString(saveFolder) + saveName + ".png";

		// 높이가 없는거 넓이로 되어있음 (압축되어 있는거)
		if (texture->mHeight < 1)
		{
			FILE* fp;
			fopen_s(&fp, saveName.c_str(), "wb");
			fwrite(texture->pcData, texture->mWidth, 1, fp);
			fclose(fp);

			return saveName;
		}
		else
		{
			ID3D11Texture2D* dest;
			D3D11_TEXTURE2D_DESC destDesc;
			ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
			destDesc.Width = texture->mWidth;
			destDesc.Height = texture->mHeight;
			destDesc.MipLevels = 1;
			destDesc.ArraySize = 1;
			destDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			destDesc.SampleDesc.Count = 1;
			destDesc.SampleDesc.Quality = 0;

			D3D11_SUBRESOURCE_DATA subResource = { 0 };
			subResource.pSysMem = texture->pcData;

			HRESULT hr;
			hr = D3D::GetDevice()->CreateTexture2D(&destDesc, &subResource, &dest);
			assert(SUCCEEDED(hr));

			D3DX11SaveTextureToFileA(D3D::GetDC(), dest, D3DX11_IFF_PNG, saveName.c_str());

			return saveName;
		}
	}
	else // 이미지가 외부에 있는 경우
	{
		string directory = Path::GetDirectoryName(String::ToString(fbxFile));
		string origin = directory + file;
		
		bool bExist = Path::ExistFile(origin);
		if (bExist == false)
			return "";
		
		string fileName = String::ToString(saveFolder) + Path::GetFileName(file);
		// false 오버라이트 하는거, true는 존재시 fail 나옴
		CopyFileA(origin.c_str(), fileName.c_str(), FALSE);

		return fileName;
	}

	return "";;
}

UINT Loader::FindBoneIndex(string name)
{
	vector<struct AsBone*>::iterator iter = bones.begin();

	for (; iter != bones.end(); iter++) {
		if ((*iter)->Name == name)
			return (*iter)->Index;
	}

	return 0;
}

void Loader::ReadKeyframeData(AsClip * clip, aiNode * node, vector<AsAniNode>& aniNodeInfos)
{
	AsKeyframe* keyframe = new AsKeyframe();
	keyframe->BoneName = node->mName.C_Str();

	for (UINT i = 0; i < clip->FrameCount; i++)
	{
		AsAniNode* aniNode = FindAniNode(aniNodeInfos, node->mName);

		AsKeyframeData frameData;
		if (aniNode != NULL) {
			frameData = aniNode->Keyframe[i];
		}
		else {
			D3DXMATRIX transform(node->mTransformation[0]);
			D3DXMatrixTranspose(&transform, &transform);

			D3DXMatrixDecompose(&frameData.Scale, &frameData.Rotation, &frameData.Translation, &transform);
			frameData.Time = i;
		}

		keyframe->Transforms.push_back(frameData);
	}

	clip->Keyframes.push_back(keyframe);

	for (UINT i = 0; i < node->mNumChildren; i++)
		ReadKeyframeData(clip, node->mChildren[i], aniNodeInfos);
}

AsAniNode* Loader::FindAniNode(vector<AsAniNode>& aniNodeInfos, aiString name)
{
	for (UINT i = 0; i < aniNodeInfos.size(); i++)
	{
		if (aniNodeInfos[i].Name == name)
			return &aniNodeInfos[i];
	}

	return NULL;
}

