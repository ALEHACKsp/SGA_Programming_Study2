#pragma once
#include "Mesh.h"

class MeshGrid : public Mesh
{
public:
	MeshGrid(Material* material, float countX, float countZ, float sizeX, float sizeZ, bool bLine = false);
	~MeshGrid();

	void CreateData() override;

	void Render() override;

private:
	float countX, countZ;
	float sizeX, sizeZ;

	bool bLine;

private:
	class GridBuffer : public ShaderBuffer
	{
	public:
		GridBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Type = 2;
			Data.Color = D3DXVECTOR3(1, 1, 1);

			Data.Spacing = 1;
			Data.Thickness = 0.1f;
		}

		struct Struct
		{
			int Type;
			D3DXVECTOR3 Color;

			int Spacing;
			float Thickness;

			float Padding[2];
		} Data;
	};

	GridBuffer* gridBuffer;
};