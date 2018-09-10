#pragma once

#include "GameRender.h"

class GameModel : public GameRender
{
public :
	GameModel(wstring matFolder, wstring matFile,
		wstring meshFolder, wstring meshFile);
	virtual ~GameModel();

	void Velocity(D3DXVECTOR3& vec);
	D3DXVECTOR3 Velocity();

	virtual void Update();
	virtual void Render();

	Model * GetModel() { return model; }

	void SetShader(Shader* shader);
	
	void SetDiffuse(float r, float g, float b, float a = 1.0f);
	void SetDiffuse(D3DXCOLOR& color);
	void SetDiffuseMap(wstring file);

	void SetSpecular(float r, float g, float b, float a = 1.0f);
	void SetSpecular(D3DXCOLOR& color);
	void SetSpecularMap(wstring file);

	void SetNormalMap(wstring file);

	void SetShininess(float val);

public:
	bool IsPickModel(
		class Camera* camera, class Viewport * viewport,
		class Perspective * perspective);

	bool IsPickModelMesh(
		class Camera* camera, class Viewport * viewport,
		class Perspective * perspective, OUT wstring& meshName);

	string GetName() { return name; }
	vector<wstring>& GetMeshNames() { return meshNames; }

protected:
	// ��ġ ��� �Լ�
	void CalcPosition();

protected:
	Model* model;
	Shader* shader;

	// �̵� �ӵ�
	D3DXVECTOR3 velocity;

	vector<D3DXMATRIX> boneTransforms;

protected:
	string name;
	vector<wstring> meshNames;

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