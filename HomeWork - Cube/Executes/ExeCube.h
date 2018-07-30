#pragma once
#include "Execute.h"

class ExeCube : public Execute
{
private:
	class ColorBuffer; // 내부 클래스라서 클래스 안에서 전방선언 해줘야함
	class UVBuffer;

public:
	ExeCube(ExecuteValues* values);
	~ExeCube();

	// Execute을(를) 통해 상속됨
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

	void CreateSRV(wstring str);
	void SetSRVNum(int num) { this->srvNum = num; }
private:
	Shader* shader;
	WorldBuffer * worldBuffer;

	ID3D11Buffer * vertexBuffer;
	ID3D11Buffer * indexBuffer;

	UINT vertexCount, indexCount;
	
	VertexTexture * vertices;
	UINT * indices;
	
	ColorBuffer * colorBuffer;
	UVBuffer * uvBuffer;

	ID3D11ShaderResourceView * srv[12];
	ID3D11ShaderResourceView * dice[6];
	float ratio[6];

	D3DXVECTOR3 location;
	D3DXVECTOR3 rotation;
	D3DXVECTOR3 scale;

	D3DXMATRIX world;
	
	function<void(wstring)> temp;
	int srvNum;
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

	class UVBuffer : public ShaderBuffer
	{
	public:
		UVBuffer()
			: ShaderBuffer(&Data, sizeof(Data))
		{
			Data.ratio = 0.5f;
		}

		~UVBuffer()
		{

		}

		struct Struct
		{
			float ratio;
			float padding[3];
		} Data;
	};
};