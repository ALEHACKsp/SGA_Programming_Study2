#pragma once
#include "Execute.h"

class ExeSphere : public Execute
{
private:
	class ColorBuffer; // 내부 클래스라서 클래스 안에서 전방선언 해줘야함

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
	D3DXMATRIX matFinal; // 최종 만들어지는 행렬 이 행렬을 월드 공간으로 씀
	D3DXVECTOR3 pos; // 반환할 나의 위치 내 주위로 공전하게 하기 위해 필요

	D3DXMATRIX matTrans, matRot, matScale; 
	// 상대도 똑같은 속도로 회전하므로 상대도 matRot은 같은걸 쓰게됨

	float rotateSpeed; // 회전속도
	float deltaRadian; // 회전값 시간에 따라 증가
	float scale;

	bool isTransduction; // 내가 공전도 할지 여부 공전 안하면 자전만함

	D3DXVECTOR3* basePos; // 생성시 지정되는 위치, 포인터로 이으면 공전할 타겟의 위치가 됨
	float dist;
	D3DXMATRIX matDist; // 공전할 타겟과의 거리
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