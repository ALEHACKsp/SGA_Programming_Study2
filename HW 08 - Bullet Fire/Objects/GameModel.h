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

	virtual void PostRender();

	// z�� ȸ�� ���Ҳ��� 2����
	void Rotate(D3DXVECTOR2 amount);

public:
	wstring Name() { return name; }

	void SetDiffuseMap(wstring file);

	Model* GetModel() { return model; }

	// �浹 �ߴ��� Ȯ���ϴ� �Լ� ��ӹ޾Ƽ� �������� ��
	virtual bool IsCollision(GameModel* model) { return false; };
	// collider ��ȯ�ϴ� �Լ� ��ӹ޾Ƽ� �������� ��
	virtual void * GetCollider() { return NULL; }

	COLLIDER_TYPE GetColliderType() { return colliderType; }
protected:
	// ��ġ ��� �Լ�
	void CalcPosition();

protected:
	Model* model;
	Shader* shader;

	// �̵� �ӵ�
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