#include "stdafx.h"
#include "BrushCommand.h"

#include "Environment\TerrainRender.h"

BrushCommand::BrushCommand(TerrainRender * terrain)
	: terrain(terrain)
	, prevHeightMapTex(NULL), heightMapTex(NULL)
	, prevHeightMapSRV(NULL), heightMapSRV(NULL)
	, id(commandId++)
{
	ID3D11Texture2D* temp = terrain->GetHeightMapTex();

	D3D11_TEXTURE2D_DESC desc;
	temp->GetDesc(&desc);
	D3D::GetDevice()->CreateTexture2D(&desc, NULL, &prevHeightMapTex);

	D3D::GetDC()->CopyResource(prevHeightMapTex, temp);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc; // �ȿ� ������ ������ 0 �ʱ�ȭ �Ұ���, �����ڰ� ��� �ȵ�
											 // union �� ����ü �ȿ� ���� ū ������� ���߰� �������� �� ���� �����ؼ� ���°�
											 // union������ padding ����� �� ����
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(prevHeightMapTex, &srvDesc, &prevHeightMapSRV);
	assert(SUCCEEDED(hr));
}

BrushCommand::~BrushCommand()
{
	SAFE_RELEASE(heightMapTex);
	SAFE_RELEASE(heightMapSRV);
	SAFE_RELEASE(prevHeightMapTex);
	SAFE_RELEASE(prevHeightMapSRV);
}

void BrushCommand::Execute()
{
	if (heightMapTex == NULL) {
		ID3D11Texture2D* temp = terrain->GetHeightMapTex();

		D3D11_TEXTURE2D_DESC desc;
		temp->GetDesc(&desc);
		D3D::GetDevice()->CreateTexture2D(&desc, NULL, &heightMapTex);

		D3D::GetDC()->CopyResource(heightMapTex, terrain->GetHeightMapTex());

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc; // �ȿ� ������ ������ 0 �ʱ�ȭ �Ұ���, �����ڰ� ��� �ȵ�
												 // union �� ����ü �ȿ� ���� ū ������� ���߰� �������� �� ���� �����ؼ� ���°�
												 // union������ padding ����� �� ����
		ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = -1;

		HRESULT hr = D3D::GetDevice()->CreateShaderResourceView(heightMapTex, &srvDesc, &heightMapSRV);
		assert(SUCCEEDED(hr));
	}
	else
		terrain->SetHeightMap(heightMapTex);
}

void BrushCommand::Undo()
{
	terrain->SetHeightMap(prevHeightMapTex);
}

void BrushCommand::Render()
{
	ImGui::Text("Brush #%d", id);

	if(ImGui::TreeNode(("Prev##" + to_string(id)).c_str())) {
		if (prevHeightMapSRV != NULL) {
			ImGui::Image(prevHeightMapSRV, ImVec2(100, 100));
		}
		ImGui::TreePop();
	}

	if(ImGui::TreeNode(("Next##" + to_string(id)).c_str())) {
		if (heightMapSRV != NULL) {
			ImGui::Image(heightMapSRV, ImVec2(100, 100));
		}
		ImGui::TreePop();
	}
}
