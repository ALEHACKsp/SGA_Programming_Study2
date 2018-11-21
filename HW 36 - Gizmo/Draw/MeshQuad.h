#pragma once

#include "Mesh.h"

class MeshQuad : public Mesh
{
public:
	MeshQuad(Material* material, D3DXVECTOR3 origin, D3DXVECTOR3 normal, D3DXVECTOR3 up, float width, float height);
	~MeshQuad();

	void CreateData() override;

	void Render() override;

private:
	D3DXVECTOR3 origin;
	D3DXVECTOR3 upperLeft;
	D3DXVECTOR3 lowerLeft;
	D3DXVECTOR3 upperRight;
	D3DXVECTOR3 lowerRight;
	D3DXVECTOR3 up;
	D3DXVECTOR3 normal;
	D3DXVECTOR3 left;
};