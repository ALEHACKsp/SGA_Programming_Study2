#pragma once

class Terrain
{
public:
	Terrain(ExecuteValues* values, Material* material);
	~Terrain();

	void Update();
	void Render();
	void PostRender();

	// position�� y���� �Ⱦ� ã�������ϴ� ��ü�� ��ġ��
	float Y(D3DXVECTOR3& position);
	bool Y(OUT D3DXVECTOR3* out, D3DXVECTOR3& position);
	bool Y(OUT D3DXVECTOR3* out);
	
	// �����Ϸ��� ���� location
	void AdjustY(D3DXVECTOR3& location);
	void AdjustColorMap(D3DXVECTOR3& location);

	void SaveColorMap(wstring fileName);
	void LoadColorMap(wstring fileName);

	void SaveHeightMap(wstring fileName);
	void LoadHeightMap(wstring fileName);

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
	Texture* colorTexture3;
	Texture* alphaTexture;

	UINT width, height;

	VertexColorTextureNormal * vertices;
	UINT * indices;

	UINT vertexCount, indexCount;

	ID3D11Buffer* vertexBuffer, *indexBuffer;

	ID3D11RasterizerState* rasterizer[2];
	ID3D11SamplerState* sampler;

	int fillMode;
	int selectBrush;
	int selectTexture;
	float capacity;

	function<void(wstring)> func;
	
	vector<D3DXCOLOR> heights;
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
			int Type; // ������ �簢������
			D3DXVECTOR3 Location; // peeking�� ��ġ �Ѱ��� ��

			int Range; // brush ����
			D3DXVECTOR3 Color; // brush ��
		} Data;
	};

	BrushBuffer* brushBuffer;
};