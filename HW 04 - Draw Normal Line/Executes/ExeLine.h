#pragma once
#include "Execute.h"

class ExeLine : public Execute
{
private:
	class ColorBuffer; // 내부 클래스라서 클래스 안에서 전방선언 해줘야함

	typedef VertexColor VertexType;
public:
	ExeLine(ExecuteValues* values, 
		D3DXVECTOR3 * startPos, D3DXVECTOR3 * endPos,
		UINT lineCount, float length = 10);
	~ExeLine();

	// Execute을(를) 통해 상속됨
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();
	
	void ResizeLength(D3DXVECTOR3 * endPos);
private:
	Shader* shader;
	WorldBuffer * worldBuffer;

	ID3D11Buffer * vertexBuffer;
	//ID3D11Buffer * indexBuffer;

	UINT vertexCount, indexCount;
	
	VertexType * vertices;
	//UINT * indices;
	
	ColorBuffer * colorBuffer;

	D3DXVECTOR3 * startPos;
	D3DXVECTOR3 * endPos;
	UINT lineCount;
private:
	// 컬러버퍼 라이트로 쓸꺼
	class ColorBuffer : public ShaderBuffer
	{
	public:
		ColorBuffer()
			: ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXVECTOR4(1, 1, 1, 1);
		}

		~ColorBuffer() 
		{

		}

		struct Struct
		{
			D3DXVECTOR4 Color;
		} Data;
	};
};