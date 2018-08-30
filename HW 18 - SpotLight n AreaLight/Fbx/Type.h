#pragma once
#include "stdafx.h"

namespace Fbx
{
	// fbx로부터 빼온 데이터 중 필요한 정보만 저장할 구조체
	struct FbxMaterial
	{
		string Name; // 저장하는건 다 string으로 할꺼 wstring binary 처리하기 귀찮음

		D3DXCOLOR Diffuse;

		string DiffuseFile;
	};

	struct FbxBoneData
	{
		int Index; // 자신의 번호
		string Name;

		int Parent;

		D3DXMATRIX LocalTransform; // local world라고 보면 됨
		D3DXMATRIX GlobalTransform; // world
	};

	struct FbxVertex
	{
		int ControlPoint;
		string MaterialName;

		ModelVertexType Vertex;
	};

	struct FbxMeshPartData
	{
		string MaterialName;

		vector<ModelVertexType> Vertices;
		vector<UINT> Indices;
	};

	struct FbxMeshData
	{
		string Name;
		int ParentBone;

		FbxMesh* Mesh;

		vector<FbxVertex *> Vertices;
		vector<FbxMeshPartData *> MeshParts;
	};
}
