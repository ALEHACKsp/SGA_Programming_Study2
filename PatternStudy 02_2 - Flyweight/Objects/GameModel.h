#pragma once

#include "GameRender.h"

#include "../Model/Model.h"
#include "../Model/ModelMesh.h"
#include "../Model/ModelBone.h"

class GameModel : public GameRender
{
public :
	GameModel(wstring matFolder, wstring matFile,
		wstring meshFolder, wstring meshFile);
	virtual ~GameModel();

	void Velocity(D3DXVECTOR3& vec);
	D3DXVECTOR3 Velocity();

	virtual void Update();
	virtual void Render();

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

	void SetAutoRotate(bool bAutoRotate) { this->bAutoRotate = bAutoRotate; }

	vector<D3DXMATRIX> BoneTransforms() { return boneTransforms; }

	// id instance set to instanceBuffer
	void SetInstance(ID3D11Buffer* instanceBuffer, UINT instanceCount);
	// boneTransform instance set to cbuffer
	void SetInstanceBuffer(vector< vector<D3DXMATRIX> >& instanceWorlds);

protected:
	// 위치 계산 함수
	void CalcPosition();

	// 자동 회전
	void AutoRotate();

protected:
	Model* model;
	Shader* shader;

	// 이동 속도
	D3DXVECTOR3 velocity;

	vector<D3DXMATRIX> boneTransforms;


	bool bAutoRotate;
	bool bInstance;

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