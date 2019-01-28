#pragma once
#include "stdafx.h"

// fbx로부터 빼온 데이터 중 필요한 정보만 저장할 구조체
struct FbxMaterial
{
	string Name; // 저장하는건 다 string으로 할꺼 wstring binary 처리하기 귀찮음

	string DiffuseFile;
	D3DXCOLOR Diffuse;

	string SpecularFile;
	D3DXCOLOR Specular;
	float SpecularExp;

	string NormalFile;
};

struct FbxBoneData
{
	int Index; // 자신의 번호
	string Name;

	int Parent;

	D3DXMATRIX LocalTransform; // local world라고 보면 됨
	D3DXMATRIX GlobalTransform; // world
};

struct FbxBlendWeight
{
	D3DXVECTOR4 Indices = D3DXVECTOR4(0, 0, 0, 0);
	D3DXVECTOR4 Weights = D3DXVECTOR4(0, 0, 0, 0);

	void Set(UINT index, UINT boneIndex, float weight)
	{
		float i = (float)boneIndex;
		float w = weight;

		switch (index)
		{
		case 0: Indices.x = i; Weights.x = w; break;
		case 1: Indices.y = i; Weights.y = w; break;
		case 2: Indices.z = i; Weights.z = w; break;
		case 3: Indices.w = i; Weights.w = w; break;
		}
	}
};

struct FbxBoneWeights
{
private:
	typedef pair<int, float> Pair;
	vector<Pair> BoneWeights;

public:
	void AddWeights(UINT boneIndex, float boneWeights)
	{
		if (boneWeights <= 0.0f) return;

		bool bAdd = false;
		vector<Pair>::iterator it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			if (boneWeights > it->second)
			{
				BoneWeights.insert(it, Pair(boneIndex, boneWeights));
				bAdd = true;

				break;
			}

			it++;
		}//while(it)

		if (bAdd == false)
			BoneWeights.push_back(Pair(boneIndex, boneWeights));
	}

	void GetBlendWeights(FbxBlendWeight& blendWeights)
	{
		for (UINT i = 0; i < BoneWeights.size(); i++)
		{
			if (i >= 4) return;

			blendWeights.Set(i, BoneWeights[i].first, BoneWeights[i].second);
		}
	}

	void Normalize()
	{
		float totalWeight = 0.0f;

		int i = 0;
		vector<Pair>::iterator it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			if (i < 4)
			{
				totalWeight += it->second;
				i++; it++;
			}
			else
				it = BoneWeights.erase(it);
		}


		float scale = 1.0f / totalWeight;

		it = BoneWeights.begin();
		while (it != BoneWeights.end())
		{
			it->second *= scale;
			it++;
		}
	}
};

struct FbxVertex
{
	int ControlPoint;
	string MaterialName;

	VertexTextureNormalTangentBlend Vertex;
};

struct FbxMeshPartData
{
	string MaterialName;

	vector<VertexTextureNormalTangentBlend> Vertices;
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
	float Time;

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

struct FbxClip
{
	string Name;

	int FrameCount;
	float FrameRate;
	float Duration; // 애니메이션의 길이 시간

	vector<FbxKeyframe *> Keyframes;
};


