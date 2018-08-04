#pragma once
#include "Execute.h"

class ExeTexture : public Execute
{
private:
	class ColorBuffer; // 내부 클래스라서 클래스 안에서 전방선언 해줘야함

public:
	ExeTexture(ExecuteValues* values);
	~ExeTexture();

	// Execute을(를) 통해 상속됨
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();
private:
	Shader* shader;
	WorldBuffer * worldBuffer;

	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer * indexBuffer;

	UINT vertexCount, indexCount;
	
	VertexTexture * vertices;
	UINT * indices;
	
	ColorBuffer * colorBuffer;

	ID3D11ShaderResourceView * srv; // 이거와 텍스처 관계는 나중에 설명해주실꺼
	// 일단 텍스처 그려주는거라고 생각 

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