#pragma once
#include "stdafx.h"

namespace Fbx
{
	// fbx�κ��� ���� ������ �� �ʿ��� ������ ������ ����ü
	struct FbxMaterial
	{
		string Name; // �����ϴ°� �� string���� �Ҳ� wstring binary ó���ϱ� ������

		D3DXCOLOR Diffuse;

		string DiffuseFile;
	};

	struct FbxBoneData
	{
		int Index; // �ڽ��� ��ȣ
		string Name;

		int Parent;

		D3DXMATRIX LocalTransform; // local world��� ���� ��
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
