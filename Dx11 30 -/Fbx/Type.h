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

	struct FbxKeyframeData
	{
		// D3DXMATRIX Trasnform // legacy�̸� �̰��ϳ� �߰��ؼ� �̰� ������ ������
		D3DXVECTOR3 Scale;
		D3DXQUATERNION Rotation;
		D3DXVECTOR3 Translation;
	};

	struct FbxKeyframe
	{
		string BoneName;
		vector<FbxKeyframeData> Transforms;
	};

	struct FbxAnimation
	{
		string Name;

		int TotalFrame;
		float FrameRate;
		float Duration; // �ִϸ��̼��� ���� �ð�

		vector<FbxKeyframe *> Keyframes;
	};

	struct FbxBoneWeights
	{
	private:
		// int ������ ���� ���� index float ������ ���� ��
		typedef pair<int, float> Pair;
		vector<Pair> BoneWeights; // �ִ� 4��

	public:
		void AddWeights(UINT boneIndex, float boneWeights)
		{

		}
	};
}
