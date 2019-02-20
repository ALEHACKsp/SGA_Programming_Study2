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
	SAFE_DELETE(importer); // importer ������ scene���� ���󰣴ٰ���
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
	//ReadSkinData();

	wstring tempFolder = saveFolder.length() < 1 ? this->saveFolder : saveFolder;
	wstring tempName = fileName.length() < 1 ? this->saveName : fileName;

	WriteMeshData(tempFolder, tempName + L".mesh");
}

void Loader::ReadMaterial()
{
	UINT count = scene->mNumMaterials;

	for (UINT i = 0; i < count; i++)
	{
		aiMaterial* asMaterial = scene->mMaterials[i];

		AsMaterial* material = new AsMaterial;

		// string���� �̷��� �ʱ�ȭ�ϸ� �ȵ�
		//ZeroMemory(material, sizeof(AsMaterial));

		// aiString assimp���� ���� string
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

		// ���⼭ �����Ҳ� ����
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

	// �����ڿ� �����ͷ� �޴°� �־ �׳� �̴�� ���� �ȴٰ���
	// transform global�� �ƴ϶� local�̶�� �� �θ� �ڽ� ���� ����ؼ� �־�����Ѵٰ���
	D3DXMATRIX transform(node->mTransformation[0]);
	D3DXMatrixTranspose(&bone->Transform, &transform);

	D3DXMATRIX temp;
	// �θ� ������
	if (parent < 0)
		D3DXMatrixIdentity(&temp);
	else
		temp = bones[parent]->Transform;

	D3DXMatrixInverse(&bone->Transform, NULL, &bone->Transform);
	bone->Transform = temp * bone->Transform;

	bones.push_back(bone);

	ReadMeshData(node, index);

	for (int i = 0; i < node->mNumChildren; i++)
		ReadBoneData(node->mChildren[i], bones.size(), index);
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

			// fbx���� layer 0���� ����
			if (mesh->HasTextureCoords(0) == true)
				memcpy(&vertex.Uv, &mesh->mTextureCoords[0][m], sizeof(D3DXVECTOR2));

			// has normal tangent�� �ִµ� aiprocess�� ������ذ�
			// �׸��� ������ null�� ���ͼ� �׷��� üũ�ص� ��
			// biTangent���� ������� �츮�� tangent�� �޾Ƽ� ����߾���
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
				0 1 2 3 ���� 4���� �簢�� �׸�
				4 5 6 7
				MeshPart MeshPart
				0 ~ 3, 4 ~ 7
				0,3    0, 3 (�̷��� ���ͼ� startVertex��ŭ �����ִ°�)

				drawIndexed �� �� baseVetex 0���� �ϱ����ؼ� ���Ѵٰ� �����
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

// �ٲ� ��ΰ� return �� ��
string Loader::WriteTexture(string file)
{
	if (file.length() < 1) return "";

	// �������� �ƴ��� �˾ƾ� ��

	// ������ �ƴϸ� null�� ����
	// �̰Ÿ��� �ٸ� ����ִµ� (�������� �ƴ���) �� ����� ���� ����ϴٰ� �Ͻ�
	const aiTexture* texture = scene->GetEmbeddedTexture(file.c_str());
	if (texture != NULL) // ����
	{
		string saveName = Path::GetFileNameWithoutExtension(file);
		saveName = String::ToString(saveFolder) + saveName + ".png";

		// ���̰� ���°� ���̷� �Ǿ����� (����Ǿ� �ִ°�)
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
	else // �̹����� �ܺο� �ִ� ���
	{
		string directory = Path::GetDirectoryName(String::ToString(fbxFile));
		string origin = directory + file;
		
		bool bExist = Path::ExistFile(origin);
		if (bExist == false)
			return "";
		
		string fileName = String::ToString(saveFolder) + Path::GetFileName(file);
		// false ��������Ʈ �ϴ°�, true�� ����� fail ����
		CopyFileA(origin.c_str(), fileName.c_str(), FALSE);

		return fileName;
	}

	return "";;
}
