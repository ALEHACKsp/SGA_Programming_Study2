#pragma once

class Material
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

	Texture* GetDiffuseMap() { return diffuseMap; }
	void SetDiffuseMap(string file, D3DX11_IMAGE_LOAD_INFO* info = NULL);
	void SetDiffuseMap(wstring file, D3DX11_IMAGE_LOAD_INFO* info = NULL);

	void PSSetBuffer();

private:
	wstring name;
	Buffer* buffer;

	bool bShaderDelete; // Shader���� �ԷµȰŸ� ���ο��� ��������� ���°�
	Shader* shader;

	Texture* diffuseMap; // ���� ������ Ŭ����

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Struct))
		{
			Data.Diffuse = D3DXCOLOR(1, 1, 1, 1);
		};

		struct Struct
		{
			D3DXCOLOR Diffuse;
		} Data;
	};
};