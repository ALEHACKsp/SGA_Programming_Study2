#pragma once
#include "Mesh.h"

class MeshGrid : public Mesh
{
public:
	MeshGrid(Material* material, float countX, float countZ, float sizeX, float sizeZ);
	~MeshGrid();

	void CreateData() override;

private:
	float countX, countZ;
	float sizeX, sizeZ;
};