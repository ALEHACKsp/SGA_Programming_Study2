#pragma once

class Terrain
{
public:
	Terrain(ExecuteValues* values, Material* material, wstring heightMap);
	~Terrain();

	void Update();
	void Render();

	// position의 y값은 안씀 찾을려고하는 물체의 위치값
	float Y(D3DXVECTOR3& position);
	bool Y(OUT D3DXVECTOR3* out, D3DXVECTOR3& position);
	bool Y(OUT D3DXVECTOR3* out);
	
	// 조정하려는 영역 location
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
	
	UINT width, height;

	VertexTextureNormal * vertices;
	UINT * indices;

	UINT vertexCount, indexCount;

	ID3D11Buffer* vertexBuffer, *indexBuffer;

	ID3D11RasterizerState* rasterizer[2];

private:
	class BrushBuffer : public ShaderBuffer
	{
	public:
		BrushBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Type = 1;
			Data.Location = D3DXVECTOR3(0, 0, 0);
			Data.Range = 2;
			Data.Color = D3DXVECTOR3(0, 1, 0);
		}

		struct Struct
		{
			int Type; // 원일지 사각형일지
			D3DXVECTOR3 Location; // peeking된 위치 넘겨줄 꺼

			int Range; // brush 범위
			D3DXVECTOR3 Color; // brush 색
		} Data;
	};

	BrushBuffer* brushBuffer;
};