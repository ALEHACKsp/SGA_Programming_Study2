#pragma once

#include "Interfaces\ICloneable.h"

class Material : public ICloneable
{
private:
	class Buffer;

public:
	Material();
	Material(wstring shaderFile);
	~Material();

	void Name(wstring val) { name = val; }
	wstring Name() { return name; }

	Shader* GetShader() { return shader; }
	void SetShader(string file);
	void SetShader(wstring file);
	void SetShader(Shader* shader);

	D3DXCOLOR* GetDiffuse() { return &buffer->Data.Diffuse; }
	void SetDiffuse(D3DXCOLOR& color) { buffer->Data.Diffuse = color; }
	void SetDiffuse(float r, float g, float b, float a = 1.0f) {
		buffer->Data.Diffuse = D3DXCOLOR(r, g, b, a);
	}

	D3DXCOLOR* GetSpecular() { return &buffer->Data.Specular; }
	void SetSpecular(D3DXCOLOR& color) { buffer->Data.Specular = color; }
	void SetSpecular(float r, float g, float b, float a = 1.0f) {
		buffer->Data.Specular = D3DXCOLOR(r, g, b, a);
	}

	Texture* GetDiffuseMap() { return diffuseMap; }
	void SetDiffuseMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetDiffuseMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);

	Texture* GetSpecularMap() { return specularMap; }
	void SetSpecularMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetSpecularMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);

	Texture* GetNormalMap() { return normalMap; }
	void SetNormalMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetNormalMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);

	float* GetShininess() { return &buffer->Data.Shininess; }
	void SetShininess(float val) { buffer->Data.Shininess = val; }

	void PSSetBuffer();

	void DeleteDiffuseMap() { SAFE_DELETE(diffuseMap); }
	void DeleteSpecularMap() { SAFE_DELETE(specularMap); }
	void DeleteNormalMap() { SAFE_DELETE(normalMap); }

public:
	void Clone(void ** clone);

private:
	wstring name;
	Buffer* buffer;

	bool bShaderDelete; // Shader에서 입력된거면 내부에서 안지우려고 쓰는거
	Shader* shader;

	Texture* diffuseMap; // 재질 관리할 클래스
	Texture* specularMap;
	Texture* normalMap;

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.Diffuse = D3DXCOLOR(1, 1, 1, 1);
			Data.Specular = D3DXCOLOR(1, 1, 1, 1);
			Data.Shininess = 1; // 보통 20정도 줌
		};

		struct Struct
		{
			D3DXCOLOR Diffuse;
			D3DXCOLOR Specular;

			float Shininess;

			float Padding[3];
		} Data;
	};
};