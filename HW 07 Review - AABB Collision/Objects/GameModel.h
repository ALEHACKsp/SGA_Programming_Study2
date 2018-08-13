#pragma once

#include "GameRender.h"

enum COLLIDER_TYPE { COLLIDER_TYPE_RECT, COLLIDER_TYPE_CIRCLE };

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

	// z축 회전 안할꺼라 2쓴거
	void Rotate(D3DXVECTOR2 amount);

public:
	wstring Name() { return name; }

	void SetDiffuseMap(wstring file);

	Model* GetModel() { return model; }

	// 충돌 했는지 확인하는 함수 상속받아서 재정의할 꺼
	virtual bool IsCollision(GameModel* model) { return false; };
	// collider 반환하는 함수 상속받아서 재정의할 꺼
	virtual void * GetCollider() { return NULL; }

	COLLIDER_TYPE GetColliderType() { return colliderType; }
protected:
	// 위치 계산 함수
	void CalcPosition();

protected:
	Model* model;
	Shader* shader;

	// 이동 속도
	D3DXVECTOR3 velocity;

	D3DXMATRIX matRotation;
	vector<D3DXMATRIX> boneTransforms;

	wstring name;

	COLLIDER_TYPE colliderType;
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