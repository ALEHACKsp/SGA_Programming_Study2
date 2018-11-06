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

	float* GetOrbitDist() { return &orbitDist; }
	bool* GetIsRotate() { return &isRotate; }

	void SetTarget(D3DXVECTOR3 targetPos) { this->targetPos = targetPos; }

	int* GetTargetNum() { return &targetNum; }
	float* GetRotateSpeed() { return &rotateSpeed; }

protected:
	// 위치 계산 함수
	void CalcPosition();

protected:
	Model* model;
	Shader* shader;

	// 이동 속도
	D3DXVECTOR3 velocity;

	vector<D3DXMATRIX> boneTransforms;

	float orbitDist;
	bool isRotate;
	D3DXVECTOR3 targetPos;
	int targetNum;
	float rotateSpeed;
	float deltaTime;
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