#pragma once

class ModelInstance
{
public:
	ModelInstance(class Model* model, wstring shaderFile);
	~ModelInstance();

	void AddWorld(D3DXMATRIX& world);

	void Ready();
	void Update();
	void Render();

private:
	class Model* model;

	UINT maxCount;
	D3DXMATRIX transforms[128];

	D3DXMATRIX worlds[64];

	ID3D11Texture2D* transTexture;
	ID3D11ShaderResourceView* transSrv;
};
