#pragma once
#include "Execute.h"

class ExeGrid : public Execute
{
private:
	class ColorBuffer; // 내부 클래스라서 클래스 안에서 전방선언 해줘야함

	typedef VertexTexture VertexType;
	
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
	Shader* shader;
	WorldBuffer * worldBuffer;

	UINT width, height;

	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer * indexBuffer;

	UINT vertexCount, indexCount;
	
	VertexType * vertices;
	UINT * indices;
	
	ColorBuffer * colorBuffer;

	// 앞으로 Texture 클래스 쓸 꺼
	Texture* texture[3];

private:
	class ColorBuffer : public ShaderBuffer
	{
	public:
		ColorBuffer()
			: ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(1, 1, 1, 1);
		}

		~ColorBuffer() 
		{

		}

		struct Struct
		{
			D3DXCOLOR Color; 
		} Data;
	};
};