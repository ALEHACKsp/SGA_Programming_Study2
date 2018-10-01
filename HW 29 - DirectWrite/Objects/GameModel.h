#pragma once

#include "GameRender.h"

class GameModel : public GameRender
{
public :
	GameModel(wstring matFolder, wstring matFile,
		wstring meshFolder, wstring meshFile);
	virtual ~GameModel();

	void Velocity(D3DXVECTOR3& vec);
	D3DXVECTOR3 Velocity();

	//void Scale(float x, float y, float z);
	//void Scale(D3DXVECTOR3 vec);

	virtual void Update();
	virtual void Render();
	virtual void PostRender();
	virtual void Render2D();

	Model * GetModel() { return model; }

	void SetShader(Shader* shader);
	
	void SetDiffuse(float r, float g, float b, float a = 1.0f);
	void SetDiffuse(D3DXCOLOR& color);
	void SetDiffuseMap(wstring file);

	void SetSpecular(float r, float g, float b, float a = 1.0f);
	void SetSpecular(D3DXCOLOR& color);
	void SetSpecularMap(wstring file);

	void SetNormalMap(wstring file);

	void SetShininess(float val);

	void Name(string name) { this->name = name; }
	string Name() { return name; }

	//void BuildBoneTree();
	void BoneTree(int boneIndex);
	void TreeRender(bool isSelect = false);
	void BoneTree2(int parentIndex, int depth);
	void BoneTree3(int index, int depth);
	void BoneTree4(int index, int depth);
	void BoneRender();

	D3DXVECTOR3 BonePosition(int boneIndex);

	void CheckPickMesh(D3DXVECTOR3& start, D3DXVECTOR3& dir);

	void ResetBoneLocal();
	void SetAnimMode(bool mode) { this->isAnimMode = mode; }
	int SelectBoneIndex() { return selectBone; }
	wstring SelectBoneName();
	D3DXMATRIX SelectBoneMatrix();

protected:
	// 위치 계산 함수
	void CalcPosition();

protected:
	Model* model;
	Shader* shader;

	// 이동 속도
	D3DXVECTOR3 velocity;

	vector<D3DXMATRIX> boneTransforms;
	vector<D3DXMATRIX> initBoneTransforms;

	class DebugDrawSphere2* debug;

	string name;
	
	DepthStencilState* depthMode[2];
	bool showBoneDebug;
	int selectBone;
	bool isAnimMode;

	vector< pair<int, wstring> > boneTrees;

private:
	class RenderBuffer : public ShaderBuffer
	{
	public:
		RenderBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Index = 0;
		}

		struct Struct
		{
			int Index;

			float Padding[3];
		} Data;
	};

	RenderBuffer* renderBuffer;
};