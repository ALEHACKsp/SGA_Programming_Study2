#pragma once

#include "stdafx.h"

struct AsMaterial
{
	string Name; // �����ϴ°� �� string���� �Ҳ� wstring binary ó���ϱ� ������

	string DiffuseFile;
	D3DXCOLOR Diffuse;

	string SpecularFile;
	D3DXCOLOR Specular;
	float SpecularExp;

	string NormalFile;

	AsMaterial()
		: Name("")
		, DiffuseFile(""), Diffuse(D3DXCOLOR(0, 0, 0, 0))
		, SpecularFile(""), Specular(D3DXCOLOR(0, 0, 0, 0)), SpecularExp(0)
		, NormalFile("") {}
};

struct AsBone
{
	int Index; // �ڽ��� ��ȣ
	string Name;

	int Parent;

	D3DXMATRIX Transform;
};

struct AsMeshPart
{
	string Name; // MeshPart�� �̸�
	string MaterialName;

	UINT StartVertex;
	UINT VertexCount;

	UINT StartIndex;
	UINT IndexCount;
};

struct AsMesh
{
	string Name;
	int ParentBone;

	aiMesh* Mesh;

	vector<VertexTextureNormalTangentBlend> Vertices;
	vector<UINT> Indices;

	vector<AsMeshPart *> MeshParts;
};
