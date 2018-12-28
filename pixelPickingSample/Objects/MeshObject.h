#pragma once
#include "GameModel.h"

class MeshObject : public GameModel
{
public:
	MeshObject(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile, D3DXCOLOR color = D3DXCOLOR(1, 1, 1, 1));
	~MeshObject();

	void Update() override;
	void Render() override;
private:
};