#pragma once
#include "stdafx.h"

// fbx�κ��� ���� ������ �� �ʿ��� ������ ������ ����ü
struct FbxMaterial
{
	string Name; // �����ϴ°� �� string���� �Ҳ� wstring binary ó���ϱ� ������

	string DiffuseFile;
	D3DXCOLOR Diffuse;

	string SpecularFile;
	D3DXCOLOR Specular;
	float SpecularExp;

	string NormalFile;
};

struct FbxBoneData
{
	int Index; // �ڽ��� ��ȣ
	string Name;

	int Parent;

	D3DXMATRIX LocalTransform; // local world��� ���� ��
	D3DXMATRIX GlobalTransform; // world
};

struct FbxVertexWeightData
{
	UINT Index;
	double Weight;
};

struct FbxControlPointData
{
	FbxVector4 Position;
	vector<FbxVertexWeightData> Datas;
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

struct FbxClip
{
	string Name;

	int FrameCount;
	float FrameRate;
	float Duration; // �ִϸ��̼��� ���� �ð�

	vector<FbxKeyframe *> Keyframes;
};


