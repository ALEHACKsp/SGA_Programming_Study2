#pragma once

#include "Systems\IExecute.h"

class TestInst : public IExecute
{
public:
	// IExecute을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual void Ready() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override;

private:
	Shader* shader;

	ID3D11Buffer* vertexBuffer[2];

	Vertex vertices[6];

	//D3DXCOLOR colors[4];
	D3DXCOLOR colors[2];
	//D3DXMATRIX world[4];
	D3DXMATRIX world[2][3];

	ID3D11Texture2D* worldData;
	ID3D11ShaderResourceView* worldDataSrv;
};
