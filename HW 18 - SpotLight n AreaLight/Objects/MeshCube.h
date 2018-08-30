#pragma once

#include "GameModel.h"

class MeshCube : public GameModel
{
public:
	MeshCube();
	~MeshCube();

	void Update() override;
	void Render() override;
	void PostRender() override;

private:
	D3DXCOLOR diffuseColor;
	D3DXCOLOR specularColor;
	float shininess;

	float val;
};