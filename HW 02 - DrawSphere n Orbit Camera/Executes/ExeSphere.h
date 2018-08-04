#pragma once
#include "Execute.h"

class ExeSphere : public Execute
{
private:
	class ColorBuffer; // 내부 클래스라서 클래스 안에서 전방선언 해줘야함

public:
	ExeSphere(ExecuteValues* values);
	~ExeSphere();

	// Execute을(를) 통해 상속됨
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();
private:
	Shader* shader;
	WorldBuffer * worldBuffer;

	float radius;
	UINT sliceCount, stackCount;

	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer * indexBuffer;

	UINT vertexCount, indexCount;
	
	VertexColor * vertices;
	UINT * indices;
	
	ColorBuffer * colorBuffer;

	int fillModeNumber;
	ID3D11RasterizerState* fillMode[2];

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