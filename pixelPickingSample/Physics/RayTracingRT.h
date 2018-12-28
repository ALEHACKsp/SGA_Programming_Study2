#pragma once

class RayTracingRT
{
public:
	RayTracingRT(ExecuteValues* values);
	~RayTracingRT();

	void PreRender(ID3D11Buffer* vertexBuffer, ID3D11Buffer* indexBuffer, UINT indexCount);
	bool Picking(OUT D3DXVECTOR3* val);

private:
	ExecuteValues* values;

	RenderTarget* renderTarget;
	Shader* shader;
	Render2D* render2D;

private:
	class Buffer : public ShaderBuffer
	{
	public:
		Buffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Width = 0;
			Data.Height = 0;
		}

		struct Struct
		{
			UINT Width;
			UINT Height;
			float Padding[2];
		} Data;
	};
	Buffer* buffer;
};