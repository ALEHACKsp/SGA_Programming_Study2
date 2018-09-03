#pragma once

class Terrain
{
public:
	Terrain(ExecuteValues* values, Material* material);
	~Terrain();

	void Update();
	void Render();

	UINT VertexCount() { return vertexCount; }
	void CoptyVertices(void* vertices);

	// position�� y���� �Ⱦ� ã�������ϴ� ��ü�� ��ġ��
	float Y(D3DXVECTOR3& position);
	bool Y(OUT D3DXVECTOR3* out, D3DXVECTOR3& position);
	bool Y(OUT D3DXVECTOR3* out);
	
	// �����Ϸ��� ���� location
	void AdjustY(D3DXVECTOR3& location);
private:
	void CreateData();
	void CreateNormalData();
	void CreateBuffer();

private:
	ExecuteValues * values;

	Material* material;
	WorldBuffer* worldBuffer;

	Texture* heightTexture;
	Texture* colorTexture;
	Texture* colorTexture2;
	Texture* alphaTexture;
	
	UINT width, height;

	VertexTextureNormal * vertices;
	UINT * indices;

	UINT vertexCount, indexCount;

	ID3D11Buffer* vertexBuffer, *indexBuffer;

	bool bWireframe;
	RasterizerState* rasterizer[2];
private:
	class BrushBuffer : public ShaderBuffer
	{
	public:
		BrushBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Type = 0;
			Data.Location = D3DXVECTOR3(0, 0, 0);
			Data.Range = 2;
			Data.Color = D3DXVECTOR3(0, 1, 0);
		}

		struct Struct
		{
			int Type; // ������ �簢������
			D3DXVECTOR3 Location; // peeking�� ��ġ �Ѱ��� ��

			int Range; // brush ����
			D3DXVECTOR3 Color; // brush ��
		} Data;
	};

	BrushBuffer* brushBuffer;

public:
	BrushBuffer* GetBrushBuffer() { return brushBuffer; }

private:
	class GridBuffer : public ShaderBuffer
	{
	public:
		GridBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Type = 2;
			Data.Color = D3DXVECTOR3(1, 1, 1);

			Data.Spacing = 1;
			Data.Thickness = 0.1f;
		}

		struct Struct
		{
			int Type;
			D3DXVECTOR3 Color;

			int Spacing;
			float Thickness;

			float Padding[2];
		} Data;
	};

	GridBuffer* gridBuffer;
};