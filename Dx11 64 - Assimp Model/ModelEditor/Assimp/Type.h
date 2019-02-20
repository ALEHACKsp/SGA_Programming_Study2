#pragma once

#include "stdafx.h"

struct AsMaterial
{
	string Name; // 저장하는건 다 string으로 할꺼 wstring binary 처리하기 귀찮음

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
	int Index; // 자신의 번호
	string Name;

	int Parent;

	D3DXMATRIX Transform;
};

struct AsMeshPart
{
	string Name; // MeshPart의 이름
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
