#pragma once

#include "../Model/Model.h"
#include "../Model/ModelMesh.h"
#include "../Model/ModelBone.h"

class GameModel : public GameRender
{
public :
	GameModel(wstring shaderFile, wstring matFile, wstring meshFile);
	~GameModel();

	virtual void Update();
	void PreRender();
	virtual void Render();

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
	void UpdateWorld() override;

protected:
	Model* model;

	vector<D3DXMATRIX> transforms;
};