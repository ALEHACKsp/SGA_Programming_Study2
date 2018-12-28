#pragma once

class Trail
{
public:
	//trailNum < 128
	Trail(D3DXMATRIX* transform, D3DXVECTOR3& translation, D3DXVECTOR3& rotationDegree, float length, UINT trailNum = 10, float time = 1);
	~Trail();

	void Update();
	void Render();
	void RenderImGui();

	void Rotation(D3DXVECTOR3& val) { rotation = val; }
	void Translation(D3DXVECTOR3& val) { translation = val; }

private:
	void GenerateTrail();
	void SetVertexBuffer();

	void Move();
	void UpdateTrailBuffer();

	void UpdateTransform();

private:
	D3DXMATRIX* world;
	D3DXMATRIX transform;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 translation;
	D3DXVECTOR3 rotation;
	
	D3DXMATRIX* circularMatrix;
	UINT head;

	float curTime;
	float time;

	float length;

	UINT vertexCount, indexCount;
	UINT* indices;
	VertexTexture* vertices;
	ID3D11Buffer* vertexBuffer, *indexBuffer;

	Texture* texture;
	Shader* shader;

	RasterizerState* cullMode[2];
	BlendState* blendMode[2];
	DepthStencilState* depthMode[2];

private:
	class TrailBuffer : public ShaderBuffer
	{
	public:
		TrailBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			for (int i = 0; i < 128; i++) {
				D3DXMatrixIdentity(&Data.Worlds[i]);
			}
			Data.Color = D3DXCOLOR(1, 1, 1, 1);

			Data.TrailNum = 1;
			Data.TrailNumInv = 1;
		}

		struct Struct
		{
			D3DXMATRIX Worlds[128];

			D3DXCOLOR Color;

			UINT TrailNum;
			float TrailNumInv;

			float Padding[2];
		} Data;
	};
	TrailBuffer* trailBuffer;
};