#pragma once

class Billboard
{
public: 
	struct VertexWorld;

public:
	Billboard();
	~Billboard();

	void ImGuiRender(bool bStart = true, bool bEnd = true);
	void Render();
	void AddInstance(D3DXVECTOR3& position);
	void AddInstance(vector<D3DXVECTOR3>& vec);

	Shader* GetShader() { return shader; }
	int GetCount() { return instanceCount; }
	void SetCount(int val) { instanceCount = val; }
	int* Factor() { return &factor; }
	void UpdateFactor(int val) { factor += val; if (factor <= 0) factor = 1; }

private:
	void CreateInstance();

private:
	Shader* shader;
	ID3D11Buffer* vertexBuffer[2];
	int instanceCount;
	int maxInstanceCount;

	vector<wstring> texturesPath;
	vector<Texture*> textures;
	TextureArray* textureArray;
	TextureArray* normalMapArray;
	
private:
	bool bRandom;
	int texId;
	int factor;
	D3DXVECTOR2 scale;

public:
	struct VertexWorld
	{
		D3DXMATRIX World;
		int textureId;
	};

private:
	vector<VertexWorld> vertices;
};
