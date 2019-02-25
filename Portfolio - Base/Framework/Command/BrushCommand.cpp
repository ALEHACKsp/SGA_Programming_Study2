#include "Framework.h"
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

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc; // 안에 포인터 있으면 0 초기화 불가능, 생성자가 없어도 안됨
											 // union 그 구조체 안에 가장 큰 사이즈로 맞추고 나머지는 그 공간 공유해서 쓰는거
											 // union에서도 padding 잡아줄 수 있음
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

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc; // 안에 포인터 있으면 0 초기화 불가능, 생성자가 없어도 안됨
												 // union 그 구조체 안에 가장 큰 사이즈로 맞추고 나머지는 그 공간 공유해서 쓰는거
												 // union에서도 padding 잡아줄 수 있음
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
