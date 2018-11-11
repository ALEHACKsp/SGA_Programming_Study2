#include "stdafx.h"
#include "Model.h"
#include "ModelBone.h"
#include "ModelMesh.h"
#include "ModelMeshPart.h"

#include "../Utilities/Xml.h"
#include "../Utilities/BinaryFile.h"

void Model::ReadMaterial(wstring folder, wstring file)
{
	Models::LoadMaterial(folder + file, &materials);
}

void Model::ReadMesh(wstring folder, wstring file)
{
	Models::LoadMesh(folder + file, &bones, &meshes);

	BindingBone();
	BindingMesh();
}

void Model::SaveMesh(wstring file)
{
	BinaryWriter* w = new BinaryWriter();
	w->Open(file);

	UINT count = bones.size();
	w->UInt(count);

	for (UINT i = 0; i < count; i++)
	{
		ModelBone* bone = bones[i];

		w->Int(bone->index);
		w->String(String::ToString(bone->name));
		w->Int(bone->parentIndex);

		w->Matrix(bone->local);
		w->Matrix(bone->global);
	}

	count = meshes.size();
	w->UInt(count);

	for (UINT i = 0; i < count; i++)
	{
		ModelMesh* mesh = meshes[i];
		w->String(String::ToString(mesh->name));
		w->Int(mesh->parentBoneIndex);

		UINT partCount = mesh->meshParts.size();
		w->UInt(partCount);
		for (int j = 0; j < partCount; j++)
		{
			ModelMeshPart* meshPart = mesh->meshParts[j];
			w->String(String::ToString(meshPart->materialName));

			// VertexData
			{
				UINT count = meshPart->vertices.size();
				w->UInt(count);
				w->Byte(&meshPart->vertices[0], sizeof(ModelVertexType) * count);
			}

			// IndexData
			{
				UINT count = meshPart->indices.size();
				w->UInt(count);
				w->Byte(&meshPart->indices[0], sizeof(UINT) * count);
			}
		}
	}

	w->Close();
	SAFE_DELETE(w);
}

void Model::SaveMaterial(wstring file)
{
	Xml::XMLDocument * document = new Xml::XMLDocument();
	Xml::XMLDeclaration *decl = document->NewDeclaration();
	document->LinkEndChild(decl);

	Xml::XMLElement* root = document->NewElement("Materials");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);
	// 버전 맞춰야되서 위에 부분 추가한거

	for (Material * material : materials)
	{
		Xml::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node); // 뒤에 붙이려고 하는거

		Xml::XMLElement * element = NULL;

		element = document->NewElement("Name");
		element->SetText(String::ToString(material->Name()).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		node->LinkEndChild(element);

		D3DXCOLOR color = *material->GetDiffuse();
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

		element = document->NewElement("DiffuseFile");
		string file = String::ToString(material->GetDiffuseMap()->GetFile());
		file = Path::GetFileName(file);
		element->SetText(file.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		file = String::ToString(material->GetSpecularMap()->GetFile());
		file = Path::GetFileName(file);
		element->SetText(file.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		file = String::ToString(material->GetNormalMap()->GetFile());
		file = Path::GetFileName(file);
		element->SetText(file.c_str());
		node->LinkEndChild(element);
	}

	document->SaveFile(String::ToString(file).c_str());
	SAFE_DELETE(document);
}

void Model::BindingBone()
{
	this->root = bones[0];
	for (ModelBone* bone : bones)
	{
		if (bone->parentIndex > -1)
		{
			bone->parent = bones[bone->parentIndex];
			bone->parent->childs.push_back(bone);
		}
		else
		{
			bone->parent = NULL;
		}
	}
}

void Model::BindingMesh()
{
	for (ModelMesh* mesh : meshes)
	{
		for (ModelBone* bone : bones)
		{
			if (mesh->parentBoneIndex == bone->index) 
			{
				mesh->parentBone = bone;
				break;
			}
		}

		for (ModelMeshPart* part : mesh->meshParts)
		{
			for (Material* material : materials)
			{
				if (material->Name() == part->materialName)
				{
					part->material = material;
					break;
				}
			}
		}

		mesh->Binding();
	}
}

///////////////////////////////////////////////////////////////////////////////
map<wstring, Models::MeshData> Models::meshDataMap;
map<wstring, vector<Material *>> Models::materialMap;
void Models::LoadMaterial(wstring file, vector<Material*>* materials)
{
	if (materialMap.count(file) < 1)
		ReadMaterialData(file);

	for (Material* material : materialMap[file])
	{
		Material* temp = NULL;
		material->Clone((void **)&temp);

		materials->push_back(temp);
	}
}

void Models::ReadMaterialData(wstring file)
{
	vector<Material *> materials;

	Xml::XMLDocument * document = new Xml::XMLDocument();

	wstring path = file;

	Xml::XMLError error = document->LoadFile(
		String::ToString(path).c_str());
	assert(error == Xml::XML_SUCCESS);

	Xml::XMLElement* root = document->FirstChildElement();
	Xml::XMLElement* matNode = root->FirstChildElement();

	do
	{
		Xml::XMLElement* node = NULL;

		Material* material = new Material();

		node = matNode->FirstChildElement();
		material->Name(String::ToWString(node->GetText()));


		node = node->NextSiblingElement();
		D3DXCOLOR dxColor;
		Xml::XMLElement * color = node->FirstChildElement();
		dxColor.r = color->FloatText();

		color = color->NextSiblingElement();
		dxColor.g = color->FloatText();

		color = color->NextSiblingElement();
		dxColor.b = color->FloatText();

		color = color->NextSiblingElement();
		dxColor.a = color->FloatText();
		material->SetDiffuse(dxColor);


		node = node->NextSiblingElement();
		wstring diffuseTexture = String::ToWString(node->GetText());
		// 폴더명만 가지고오는 함수
		wstring directory = Path::GetDirectoryName(file);
		if (diffuseTexture.length() > 0)
			material->SetDiffuseMap(directory + diffuseTexture);

		node = node->NextSiblingElement();
		wstring specularTexture = String::ToWString(node->GetText());
		// 폴더명만 가지고오는 함수
		directory = Path::GetDirectoryName(file);
		if (specularTexture.length() > 0)
			material->SetSpecularMap(directory + specularTexture);

		node = node->NextSiblingElement();
		wstring normalTexture = String::ToWString(node->GetText());
		// 폴더명만 가지고오는 함수
		directory = Path::GetDirectoryName(file);
		if (normalTexture.length() > 0)
			material->SetNormalMap(directory + normalTexture);

		materials.push_back(material);
		matNode = matNode->NextSiblingElement();
	} while (matNode != NULL);

	materialMap[file] = materials;
	SAFE_DELETE(document);
}

void Models::LoadMesh(wstring file, vector<class ModelBone*>* bones, vector<class ModelMesh*>* meshes)
{
	if (meshDataMap.count(file) < 1)
		ReadMeshData(file);

	MeshData data = meshDataMap[file];
	for (size_t i = 0; i < data.Bones.size(); i++)
	{
		ModelBone* bone = NULL;
		data.Bones[i]->Clone((void **)&bone);

		bones->push_back(bone);
	}

	for (size_t i = 0; i < data.Meshes.size(); i++)
	{
		ModelMesh* mesh = NULL;
		data.Meshes[i]->Clone((void **)&mesh);

		meshes->push_back(mesh);
	}
}

void Models::ReadMeshData(wstring file)
{
	BinaryReader* r = new BinaryReader();
	r->Open(file);

	vector<ModelBone*> bones;
	vector<ModelMesh*> meshes;

	UINT count = 0;
	count = r->UInt();

	for (UINT i = 0; i < count; i++)
	{
		ModelBone* bone = new ModelBone();

		bone->index = r->Int();
		bone->name = String::ToWString(r->String());
		bone->parentIndex = r->Int();

		bone->local = r->Matrix();
		bone->global = r->Matrix();

		bones.push_back(bone);
	}

	count = r->UInt();
	for (UINT i = 0; i < count; i++)
	{
		ModelMesh* mesh = new ModelMesh();
		mesh->name = String::ToWString(r->String());
		mesh->parentBoneIndex = r->Int();

		UINT partCount = r->UInt();
		for (UINT k = 0; k < partCount; k++)
		{
			ModelMeshPart* meshPart = new ModelMeshPart();
			meshPart->parent = mesh;
			meshPart->materialName = String::ToWString(r->String());

			//VertexData
			{
				UINT count = r->UInt();
				meshPart->vertices.assign(count, ModelVertexType());

				void* ptr = (void *)&(meshPart->vertices[0]);
				r->Byte(&ptr, sizeof(ModelVertexType) * count);
			}

			//IndexData
			{
				UINT count = r->UInt();
				meshPart->indices.assign(count, UINT());

				void* ptr = (void *)&(meshPart->indices[0]);
				r->Byte(&ptr, sizeof(UINT) * count);
			}

			mesh->meshParts.push_back(meshPart);
		} // for(k)

		meshes.push_back(mesh);
	} // for(i)

	r->Close();
	SAFE_DELETE(r);

	MeshData data;
	data.Bones.assign(bones.begin(), bones.end());
	data.Meshes.assign(meshes.begin(), meshes.end());

	meshDataMap[file] = data;
}
