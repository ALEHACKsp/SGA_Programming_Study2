#include "stdafx.h"
#include "Billboard.h"

Billboard::Billboard(ExecuteValues * values, 
	wstring shaderFile, wstring diffuseFile)
	: GameModel(Materials + L"/Meshes/", L"Quad.material",
		Models + L"/Meshes/", L"Quad.mesh")
	, values(values), bFixedY(false)
{
	for (Material * material : model->Materials())
	{
		material->SetShader(shaderFile);
		material->SetDiffuseMap(diffuseFile);
	}

	cullMode[0] = new RasterizerState();
	cullMode[1] = new RasterizerState();
	cullMode[1]->FrontCounterClockwise(true);
}

Billboard::~Billboard()
{
	SAFE_DELETE(cullMode[0]);
	SAFE_DELETE(cullMode[1]);
}

void Billboard::Update()
{
	// ���� ������� ���� ��
	D3DXMATRIX R;
	values->MainCamera->Matrix(&R);
	// ��� ���� NULL�� ��Ľ���
	D3DXMatrixInverse(&R, NULL, &R);

	// �� ������ matrix ������ �� ���ֽǲ�
	float x = asinf(-R._32);
	float y = atan2f(R._31, R._33);
	float z = atan2f(R._12, R._22);

	if (bFixedY == true)
		Rotation(x, 0, z);
	else
		Rotation(x, y, z);

	__super::Update();
}

void Billboard::Render()
{
	cullMode[1]->RSSetState();

	__super::Render();

	cullMode[0]->RSSetState();
}
