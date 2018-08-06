#pragma once
#include "Execute.h"

class ExeSphere : public Execute
{
private:
	class ColorBuffer; // ���� Ŭ������ Ŭ���� �ȿ��� ���漱�� �������

public:
	ExeSphere(ExecuteValues* values);
	~ExeSphere();

	// Execute��(��) ���� ��ӵ�
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

	D3DXMATRIX* GetMatrix() { return &mat; }
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

	D3DXMATRIX mat;
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