#pragma once

class Sky
{
public:
	Sky();
	~Sky();

	void Update();
	void Render();

private:
	Material* material;
	MeshSphere* sphere;

	D3DXCOLOR apex;
	D3DXCOLOR center;
	float height;

	ID3DX11EffectVectorVariable* apexVariable;
	ID3DX11EffectVectorVariable* centerVariable;
	ID3DX11EffectScalarVariable* heightVariable;
};