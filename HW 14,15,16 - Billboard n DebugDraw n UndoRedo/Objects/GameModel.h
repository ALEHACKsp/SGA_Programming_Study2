#pragma once

#include "GameRender.h"

class GameModel : public GameRender
{
public:
	GameModel(wstring matFolder, wstring matFile,
		wstring meshFolder, wstring meshFile);
	virtual ~GameModel();

	void Velocity(D3DXVECTOR3& vec);
	D3DXVECTOR3 Velocity();

	virtual void Update();
	virtual void Render();

	Model * GetModel() { return model; }
	
public:
	bool IsPickModel(
		class Camera* camera, class Viewport * viewport,
		class Perspective * perspective);

	bool IsPickModelMesh(
		class Camera* camera, class Viewport * viewport,
		class Perspective * perspective, OUT wstring& meshName);

	vector<wstring>& GetMeshNames() { return meshNames; }

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