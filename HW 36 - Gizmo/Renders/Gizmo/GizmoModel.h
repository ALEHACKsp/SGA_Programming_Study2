#pragma once

#include "Objects\GameRender.h"

class GizmoModel : public GameRender
{
public:
	enum class Kind { Rotate, Translate, Scale };

public:
	GizmoModel(Kind kind);
	~GizmoModel();

	void CreateVertex();

	void Update();
	void Render();

	void SetColor(D3DXCOLOR color);

private:
	Kind kind;

	Shader* shader;

	VertexTextureNormal* vertices;

	UINT vertexCount;
	UINT indexCount;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	WorldBuffer* worldBuffer;

private:
	class ColorBuffer : public ShaderBuffer
	{
	public:
		ColorBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(1, 1, 1, 1);
		}

		struct Struct
		{
			D3DXCOLOR Color;
		} Data;
	};

	ColorBuffer* colorBuffer;
};