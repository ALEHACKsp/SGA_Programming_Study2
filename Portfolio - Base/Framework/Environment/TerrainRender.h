#pragma once

class TerrainRender
{
public:
	static const float MaxDistance;
	static const float MinDistance;
	static const float MaxTessellation;
	static const float MinTessellation;

public:
	TerrainRender(class Terrain* terrain);
	~TerrainRender();

	void Initialize();
	void Ready(Material* material);

	void Update();
	void PreRender();
	void Render();

	bool Pick(OUT D3DXVECTOR3* val);

	void LoadHeightMap(wstring fileName);
	void SaveHeightMap(wstring fileName);

	void LoadBillboard(wstring fileName);
	void SaveBillboard(wstring fileName);

	void ImGuiRender();

	ID3D11Texture2D* GetHeightMapTex() { return heightMapTex; }
	void SetHeightMap(ID3D11Texture2D* tex);

private:
	void CalcAllPatchBoundsY();
	void CalcPatchBoundsY(UINT row, UINT col);
	void BuildQuadPatchVB();
	void BuildQuadPatchIB();
	//void BuildPatches();

	void CreateBlendMap();
	void SmoothBlendMap();

	void AdjustY();
	void AddBillboard();

	void UpdateHeightMap();

	void Undo();
	void Redo();

private:
	D3DXMATRIX world;
	ID3D11Buffer* cBuffer;
	ID3D11Buffer* brushBuffer;

	class Terrain* terrain;

	ID3D11Buffer* quadPatchVB;
	ID3D11Buffer* quadPatchIB;

	vector<class Patch *> patches;

	TextureArray* layerMapArray;
	ID3D11ShaderResourceView* layerMapArraySRV;

	ID3D11Texture2D* blendMapTex;
	ID3D11ShaderResourceView* blendMapSRV;
	ID3D11UnorderedAccessView* blendMapUAV;
	Shader* blendMapComputeShader;

	ID3D11Texture2D* heightMapTex;
	ID3D11ShaderResourceView* heightMapSRV;
	ID3D11UnorderedAccessView* heightMapUAV;

	UINT patchVerticesCount;
	UINT patchQuadFacesCount;
	UINT patchVertexRows;
	UINT patchVertexCols;

	Material* material;
	vector<D3DXVECTOR2> patchBoundsY;
	
	vector<VertexColor> bvhVertices;
	vector<WORD> bvhIndices;
	ID3D11Buffer* bvhVertexBuffer;
	ID3D11Buffer* bvhIndexBuffer;

	UINT aabbCount;

	class Frustum* frustum;

	bool bWireFrame;
	int selectSRV;
	int selectEdit;

private:
	Shader* fastPickingShader;
	Render2D* render2D;

	D3DXVECTOR3 pickPos;

	Shader* adjustHeightMapComputeShader;
	RenderTargetView* rtv;
	DepthStencilView* dsv;

	function<void(wstring)> func;

private:
	class Billboard* billboard;
	set<pair<int, int>> checkSet;

private:
	stack<class Command*> undoStack;
	stack<class Command*> redoStack;

private:
	ID3DX11EffectConstantBuffer* constantBufferVariable;

	ID3DX11EffectMatrixVariable* worldVariable;

	ID3DX11EffectShaderResourceVariable* heightMapVariable;
	ID3DX11EffectShaderResourceVariable* layerMapArrayVariable;
	ID3DX11EffectShaderResourceVariable* blendMapVariable;

private:
	struct BrushBuffer
	{
		int Type;
		D3DXVECTOR3 Location;

		int Range;
		D3DXVECTOR3 Color;

		float Capacity;
		float Padding[3];
	} brush;

private:
	struct Buffer
	{
		D3DXCOLOR FogColor;

		float FogStart;
		float FogRange;

		float MinDistance;
		float MaxDistance;
		float MinTessellation;
		float MaxTessellation;

		float TexelCellSpaceU;
		float TexelCellSpaceV;
		float WorldCellSpace;

		D3DXVECTOR2 TexScale = D3DXVECTOR2(66.0f, 66.0f);
		float Padding;

		D3DXPLANE WorldFrustumPlanes[6];

	} buffer;
};
