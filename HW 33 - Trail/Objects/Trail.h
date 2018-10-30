#pragma once

#include "GameRender.h"

class Trail : public GameRender
{
public:
	Trail(int count = 32);
	virtual ~Trail();

	void Update();
	virtual void Render();
	void PostRender();

	void SetRender(bool bRender);
	void SetGameModel(class GameModel * model) { this->model = model; }
	void SetBoneName(wstring name) { boneName = name; bBone = true; }
	void ShiftMatrix();

private:

	WorldBuffer* worldBuffer;
	ID3D11Buffer* vertexBuffer;
	Shader* shader;
	
	Texture* texture;
	RasterizerState* cullMode[2];
	DepthStencilState* depthMode[2];
	BlendState* blendMode[2];

	class GameModel* model;
	wstring boneName;
	D3DXMATRIX matrix;

	int count;
	bool bBone;
	bool bCull;
	bool bDepth;

	bool bRender;

	float changeTime;
	float deltaTime;
private:
	class VsBuffer : public ShaderBuffer
	{
	public:
		VsBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			for (int i = 0; i < 128; i++) {
				D3DXMatrixIdentity(&Data.Worlds[i]);
			}

			Data.Count = 1;
		}

		struct Struct
		{
			D3DXMATRIX Worlds[128];

			int Count;
			float Padding[3];
		} Data;
	};

	VsBuffer* vsBuffer;

	class PsBuffer : public ShaderBuffer
	{
	public:
		PsBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(1, 0, 0, 1);

			Data.Select = 1;
		}

		struct Struct
		{
			D3DXCOLOR Color;

			int Select;
			float Padding[3];
		} Data;
	};

	PsBuffer* psBuffer;
};