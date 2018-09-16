#include "stdafx.h"
#include "ExportAnim.h"

#include "../Fbx/Exporter.h"

#include "../Objects/GameModel.h"

ExportAnim::ExportAnim(ExecuteValues * values)
	:Execute(values)
{
	Fbx::Exporter* exporter = NULL;

	// Kachujin Mesh
	exporter = new Fbx::Exporter(Assets + L"Kachujin/Mesh.fbx");
	exporter->ExportMaterial(Models + L"Kachujin/", L"Kachujin.material");
	//exporter->ExportMesh(Models + L"Kachujin/", L"Kachujin.mesh", D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	//exporter->ExportMesh(Models + L"Kachujin/", L"Kachujin.mesh", D3DXVECTOR3(2.0f, 2.0f, 2.0f));
	exporter->ExportMesh(Models + L"Kachujin/", L"Kachujin.mesh", D3DXVECTOR3(0.1f, 0.1f, 0.1f));
	SAFE_DELETE(exporter);

	values->MainCamera->Position(0.0f, 125.0f, -800.0f);

	exporter = new Fbx::Exporter(Assets + L"Kachujin/Samba_Dancing.fbx");
	exporter->ExportAnimation(Models + L"Kachujin/", L"Samba_Dance.anim", 0, D3DXVECTOR3(1.0f, 1.0f, 1.0f));

	SAFE_DELETE(exporter);

}

ExportAnim::~ExportAnim()
{

}

void ExportAnim::Update()
{

}

void ExportAnim::PreRender()
{
	
}

void ExportAnim::Render()
{

}

void ExportAnim::PostRender()
{
	
}

void ExportAnim::ResizeScreen()
{
}

