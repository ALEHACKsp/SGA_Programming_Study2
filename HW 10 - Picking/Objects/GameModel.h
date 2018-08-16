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

	bool IsMouseCollision(
		class Camera* camera, class Viewport * viewport,
		class Perspective * perspective);

	virtual void * GetCollider() { return NULL; }
	COLLIDER_TYPE GetColliderType() { return colliderType; }

	Model * GetModel() { return model; }
	
	string GetName() { return name; }

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

	COLLIDER_TYPE colliderType;

	class Line * line;

	string name;
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