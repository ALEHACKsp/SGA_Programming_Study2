#pragma once
#include "Execute.h"

class ExeSphere : public Execute
{
private:
	class ColorBuffer; // ���� Ŭ������ Ŭ���� �ȿ��� ���漱�� �������

public:
	~ExeSphere();

	void Update();
	void Render();
	void PreRender();
	void PostRender();
	void ResizeScreen();

#if CASE
	ExeSphere(ExecuteValues* values);
#else
	ExeSphere(ExecuteValues* values, D3DXVECTOR3* basePos,
		float scale = 1, float rotateSpeed = 1,
		bool isTransduction = false, float dist = 0,
		D3DXCOLOR color = D3DXCOLOR(1,1,1,1));

	//void Position(D3DXVECTOR3 * vec) {
	//	*vec = pos;
	//}
	D3DXVECTOR3 * GetPosition() {
		return &pos;
	}
#endif

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

#if CASE

#else
	D3DXMATRIX matFinal; // ���� ��������� ��� �� ����� ���� �������� ��
	D3DXVECTOR3 pos; // ��ȯ�� ���� ��ġ �� ������ �����ϰ� �ϱ� ���� �ʿ�

	D3DXMATRIX matTrans, matRot, matScale; 
	// ��뵵 �Ȱ��� �ӵ��� ȸ���ϹǷ� ��뵵 matRot�� ������ ���Ե�

	float rotateSpeed; // ȸ���ӵ�
	float deltaRadian; // ȸ���� �ð��� ���� ����
	float scale;

	bool isTransduction; // ���� ������ ���� ���� ���� ���ϸ� ��������

	D3DXVECTOR3* basePos; // ������ �����Ǵ� ��ġ, �����ͷ� ������ ������ Ÿ���� ��ġ�� ��
	float dist;
	D3DXMATRIX matDist; // ������ Ÿ�ٰ��� �Ÿ�
#endif
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