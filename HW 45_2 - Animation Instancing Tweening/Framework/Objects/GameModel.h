#pragma once

#include "../Model/Model.h"
#include "../Model/ModelMesh.h"
#include "../Model/ModelBone.h"

class GameModel : public GameRender
{
public :
	GameModel(wstring shaderFile, wstring matFile, wstring meshFile);
	virtual ~GameModel();

	virtual void Ready();

	virtual void Update() override;
	void PreRender();
	virtual void Render() override;

	Model * GetModel() { return model; }

	#pragma region Material
	void SetShader(wstring shaderFile);

	void SetDiffuse(float r, float g, float b, float a = 1.0f);
	void SetDiffuse(D3DXCOLOR& color);
	void SetDiffuseMap(wstring file);

	void SetSpecular(float r, float g, float b, float a = 1.0f);
	void SetSpecular(D3DXCOLOR& color);
	void SetSpecularMap(wstring file);

	void SetNormalMap(wstring file);

	void SetShininess(float val);
	#pragma endregion

protected:
	virtual void UpdateWorld() override;
	void UpdateVertex();

	virtual void UpdateTransforms();
	void MappedBoneBuffer();

protected:
	wstring shaderFile;
	wstring matFile;
	wstring meshFile;
	Model* model;

	UINT pass;

	Shader* shader;

	ID3D11Buffer* boneBuffer;
	ID3D11ShaderResourceView* boneSrv;

	//vector<D3DXMATRIX> transforms;

	D3DXMATRIX* boneTransforms;
	D3DXMATRIX* renderTransforms;

private:
	ID3DX11EffectScalarVariable* boneIndexVariable;
	ID3DX11EffectShaderResourceVariable* vertexVariable;
	ID3DX11EffectUnorderedAccessViewVariable* resultVariable;

	ID3DX11EffectShaderResourceVariable* boneBufferVariable;
};