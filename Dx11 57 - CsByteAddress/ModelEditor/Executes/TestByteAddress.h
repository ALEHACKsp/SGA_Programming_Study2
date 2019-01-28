#pragma once

#include "Systems\IExecute.h"

class TestByteAddress : public IExecute
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
	D3DXVECTOR4 datas[128];
	VertexTextureNormalTangentBlend datas2[128];

	Shader* shader;

	ID3D11Buffer* input;
	ID3D11ShaderResourceView* inputSRV;

	ID3D11Buffer* output;
	ID3D11Buffer* result;
	ID3D11UnorderedAccessView* outputUAV;

	ID3D11Buffer* dest;
};
