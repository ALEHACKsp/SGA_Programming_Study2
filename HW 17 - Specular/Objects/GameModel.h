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

	virtual void Update();
	virtual void Render();
	virtual void PostRender();

	Model * GetModel() { return model; }

	void SetShader(Shader* shader);
	
	void SetDiffuse(float r, float g, float b, float a = 1.0f);
	void SetDiffuse(D3DXCOLOR& color);
	void SetDiffuseMap(wstring file);

	void SetSpecular(float r, float g, float b, float a = 1.0f);
	void SetSpecular(D3DXCOLOR& color);
	void SetSpecularMap(wstring file);

	void SetShininess(float val);

public:
	bool IsPickModel(
		class Camera* camera, class Viewport * viewport,
		class Perspective * perspective);

	bool IsPickModelMesh(
		class Camera* camera, class Viewport * viewport,
		class Perspective * perspective, OUT wstring& meshName);

	vector<wstring>& GetMeshNames() { return meshNames; }
	
	float* GetOrbitDist() { return &orbitDist; }
	//D3DXVECTOR3* GetOrbitDist() { return &orbitDist; }
	bool* GetIsOrbit() { return &isOrbit; }

	void SetTarget(D3DXVECTOR3 targetPos) { this->targetPos = targetPos; }

	int* GetTargetNum() { return &targetNum; }
	float* GetRotateSpeed() { return &rotateSpeed; }
	//D3DXVECTOR3* GetRotateSpeed() { return &rotateSpeed; }
private:
	D3DXCOLOR diffuseColor;
	D3DXCOLOR specularColor;
	float shininess;
protected:
	// 위치 계산 함수
	void CalcPosition();

protected:
	Model* model;
	Shader* shader;

	// 이동 속도
	D3DXVECTOR3 velocity;

	vector<D3DXMATRIX> boneTransforms;

	vector<wstring> meshNames;

	float orbitDist;
	//D3DXVECTOR3 orbitDist;
	bool isOrbit;
	D3DXVECTOR3 targetPos;
	int targetNum;
	float rotateSpeed;
	//D3DXVECTOR3 rotateSpeed;
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