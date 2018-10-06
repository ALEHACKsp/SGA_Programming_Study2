#pragma once

class WorldBuffer : public ShaderBuffer
{
public:
	WorldBuffer();

	void SetMatrix(D3DXMATRIX mat);

	struct Struct
	{
		D3DXMATRIX World;
	};

private:
	Struct Data;
};

class ViewProjectionBuffer : public ShaderBuffer
{
public:
	ViewProjectionBuffer();

	void SetView(D3DXMATRIX mat);

	void SetProjection(D3DXMATRIX mat);

	struct Struct
	{
		D3DXMATRIX View;
		D3DXMATRIX Projection;
		D3DXMATRIX ViewInverse;
	};

private:
	Struct Data;
};

class LightBuffer : public ShaderBuffer
{
public:
	LightBuffer();

	struct Struct
	{
		D3DXVECTOR3 Direction;
		float Padding;

		D3DXCOLOR Color;
		
		float Overcast;
		float Intensity;
		float Padding2[2];
	};

	Struct Data;
};