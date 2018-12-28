#pragma once
#include "GameRender.h"
#include "./Model/Model.h"
#include "./Model/ModelMesh.h"
#include "./Model/ModelBone.h"

class Collider;
class Gizmo;
class Model;
class GameModel : public GameRender
{
public:
	GameModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile);
	virtual ~GameModel();

	void Velocity(D3DXVECTOR3& vec);
	D3DXVECTOR3 Velocity();

	virtual void Update();
	virtual void Render();

	virtual void DebugRender();

	void BoundingBox(D3DXVECTOR3 * min, D3DXVECTOR3 * max);

	Collider* GetCollider() { return collider; }
	Gizmo* GetGizmo() { return gizmo; }

	Model* GetModel() { return model; }

	void SetShader(Shader* shader);
	void SetShader(wstring file, string vsName = "VS", string psName = "PS");

	void SetDiffuse(float r, float g, float b, float a = 1.0f);
	void SetDiffuse(D3DXCOLOR& color);
	D3DXCOLOR* GetDiffuse(UINT index = 0);
	void SetDiffuseMap(wstring file);
	Texture* GetDiffuseMap(UINT index = 0);

	void SetSpecular(float r, float g, float b, float a = 1.0f);
	void SetSpecular(D3DXCOLOR& color);
	D3DXCOLOR* GetSpecular(UINT index = 0);
	void SetSpecularMap(wstring file);
	Texture* GetSpecularMap(UINT index = 0);

	void SetNormalMap(wstring file);
	Texture* GetNormalMap(UINT index = 0);

	void SetShininess(float val);
	float* GetShininess(UINT index = 0);

	void SetInstance(ID3D11Buffer* instanceBuffer, UINT instanceCount);
	void SetInstanceBuffer(vector< vector<D3DXMATRIX> >& instanceWorlds);

	vector<D3DXMATRIX>* GetBoneTransforms() { return &boneTransforms; }
	void GetBoneTransformByName(wstring name, D3DXMATRIX* transformOut);

protected:
	void CalcPosition();

protected:
	Model* model;
	Shader* shader;

	D3DXVECTOR3 velocity;
	
	vector<D3DXMATRIX> boneTransforms;

	Collider* collider;
	Gizmo* gizmo;

	bool bInstance;
private:
	class RenderBuffer : public ShaderBuffer
	{
	public:
		RenderBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Index = 0;
		}
		struct Struct
		{
			int Index;
			float Padding[3];
		} Data;
	};

	RenderBuffer* renderBuffer;
};