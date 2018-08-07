#pragma once
#include "Execute.h"

class ExeGrid : public Execute
{
private:
	typedef VertexTextureNormal VertexType;
	
public:
	ExeGrid(ExecuteValues* values);
	~ExeGrid();

	// Execute을(를) 통해 상속됨
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();
	
private:
	Material* material;
	WorldBuffer * worldBuffer;

	UINT width, height;

	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer * indexBuffer;

	UINT vertexCount, indexCount;
	
	VertexType * vertices;
	UINT * indices;

	// 앞으로 Texture 클래스 쓸 꺼
	Texture* texture[2];

private:
	void CreateNormal();
};