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

	struct FbxKeyframeData
	{
		// D3DXMATRIX Trasnform // legacy이면 이거하나 추가해서 이거 가지고 움직임
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
		float Duration; // 애니메이션의 길이 시간

		vector<FbxKeyframe *> Keyframes;
	};

	struct FbxBoneWeights
	{
	private:
		// int 영향을 받을 본의 index float 영향을 받을 값
		typedef pair<int, float> Pair;
		vector<Pair> BoneWeights; // 최대 4개

	public:
		void AddWeights(UINT boneIndex, float boneWeights)
		{

		}
	};
}
