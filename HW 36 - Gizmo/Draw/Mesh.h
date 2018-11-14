#pragma once

#include "Objects\GameRender.h"

class Mesh : public GameRender
{
public:
	Mesh(Material* material);
	virtual ~Mesh();

public:
	virtual void Render() override;

protected:
	virtual void CreateData() = 0;
	void CreateBuffer();

protected:
	VertexTextureNormalTangent* vertices;
	UINT* indices;

	UINT vertexCount, indexCount;

private:
	Material* material;

	WorldBuffer* worldBuffer;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
};