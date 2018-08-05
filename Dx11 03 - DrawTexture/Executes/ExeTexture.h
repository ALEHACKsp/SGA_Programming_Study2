#pragma once
#include "Execute.h"

class ExeTexture : public Execute
{
private:
	class ColorBuffer; // ���� Ŭ������ Ŭ���� �ȿ��� ���漱�� �������

public:
	ExeTexture(ExecuteValues* values);
	~ExeTexture();

	// Execute��(��) ���� ��ӵ�
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

	ID3D11ShaderResourceView * srv; // �̰ſ� �ؽ�ó ����� ���߿� �������ֽǲ�
	// �ϴ� �ؽ�ó �׷��ִ°Ŷ�� ���� 

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