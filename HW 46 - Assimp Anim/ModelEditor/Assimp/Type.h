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

	vector<VertexTextureNormalTangentBlend> Vertices;
	vector<UINT> Indices;

	vector<AsMeshPart *> MeshParts;
};

struct AsKeyframeData
{
	float Time;

	D3DXVECTOR3 Scale;
	D3DXQUATERNION Rotation;
	D3DXVECTOR3 Translation;
};

struct AsKeyframe
{
	string BoneName;
	vector<AsKeyframeData> Transforms;
};

struct AsAniNode 
{
	vector<AsKeyframeData> Keyframe;
	aiString Name;
};

struct AsClip
{
	string Name;

	UINT FrameCount;
	float FrameRate;
	float Duration; // 애니메이션의 길이 시간

	vector<AsKeyframe *> Keyframes;
};

struct AsBlendWeight
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

struct AsBoneWeights
{
private:
	// int 영향을 받을 본의 index float 영향을 받을 값
	typedef pair<int, float> Pair;
	vector<Pair> BoneWeights; // 최대 4개

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
		} // while(it)

		if (bAdd == false)
			BoneWeights.push_back(Pair(boneIndex, boneWeights));
	}

	void GetBlendWeights(AsBlendWeight& blendWeights)
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