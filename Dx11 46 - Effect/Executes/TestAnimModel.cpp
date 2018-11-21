#include "stdafx.h"
#include "TestAnimModel.h"

TestAnimModel::TestAnimModel()
{
	//Context::Get()->GetGlobalLight()->Direction = D3DXVECTOR3(1, 1, 1);

	models.push_back(new GameAnimModel
	(
		Shaders + L"052_Model.fx",
		Models + L"Kachujin/", L"Kachujin.material",
		Models + L"Kachujin/", L"Kachujin.mesh"
	));

	models.back()->Scale(0.0125f, 0.0125f, 0.0125f);
	models.back()->AddClip(Models + L"Kachujin/Samba_Dance.anim");
	models.back()->Play(0, true);
}

TestAnimModel::~TestAnimModel()
{	
	for (GameAnimModel* model : models)
		SAFE_DELETE(model);
}

void TestAnimModel::Update()
{
	for (GameAnimModel* model : models)
		model->Update();
}

void TestAnimModel::PreRender()
{

}

void TestAnimModel::Render()
{
	for (GameAnimModel* model : models)
		model->Render();
}

void TestAnimModel::PostRender()
{

}
