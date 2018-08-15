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

	// z�� ȸ�� ���Ҳ��� 2����
	void Rotate(D3DXVECTOR2 amount);

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

////////////////////////////////////
//	// �ӽ÷� ����°� �����ϳ� �������ֽǲ�
//private:
//	ModelBone* turretBone;

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