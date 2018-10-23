#pragma once
#include "Execute.h"

class TestAmbient : public Execute
{
public:
	TestAmbient(ExecuteValues* values);
	~TestAmbient();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	class MeshPlane* plane;
	class MeshCube* cube;
	class MeshSphere* sphere;
	class MeshSphere* sphere2;
	class MeshBunny* bunny;

	Shader* shader;

private:
	class AmbientBuffer : public ShaderBuffer
	{
	public:
		AmbientBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(0.85f, 0.8f, 0.5f, 1.0f);

			Data.Floor = D3DXVECTOR3(0, 1, 0);
			Data.Ceil = D3DXVECTOR3(1, 1, 1);
		}

		struct Struct
		{
			D3DXCOLOR Color;

			D3DXVECTOR3 Floor;
			float Padding;
			D3DXVECTOR3 Ceil;
			float Padding2;
		} Data;
	};

	AmbientBuffer * ambientBuffer;

	class SpecularBuffer : public ShaderBuffer
	{
	public:
		SpecularBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Exp = 10;
			Data.Intensity = 2;
		}

		struct Struct
		{
			float Exp;
			float Intensity;

			float Padding[2];
		} Data;
	};

	SpecularBuffer * specularBuffer;
};