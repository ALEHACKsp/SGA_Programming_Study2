#include "stdafx.h"
#include "TestModel.h"

TestModel::TestModel()
{
	//models.push_back(new GameModel
	//(
	//	Shaders + L"052_Model.fx",
	//	Models + L"Meshes/", L"StanfordBunny.material",
	//	Models + L"Meshes/", L"StanfordBunny.mesh"
	//));
	//models.back()->Scale(0.01f, 0.01f, 0.01f);
	//models.back()->SetDiffuseMap(Textures + L"Red.png");

	for (int i = 0; i < 20; i++) {

		models.push_back(new GameModel
		(
			Shaders + L"052_Model.fx",
			Models + L"Meshes/", L"Tank.material",
			Models + L"Meshes/", L"Tank.mesh"
		));

		models.back()->Position(0, 0, i * 50);
	}

	//models.push_back(new GameModel
	//(
	//	Shaders + L"052_Model.fx",
	//	Models + L"Kachujin/", L"Kachujin.material",
	//	Models + L"Kachujin/", L"Kachujin.mesh"
	//));
}

TestModel::~TestModel()
{	
	for (GameModel* model : models)
		SAFE_DELETE(model);
}

void TestModel::Update()
{
	for (GameModel* model : models)
		model->Update();
}

void TestModel::PreRender()
{

}

void TestModel::Render()
{
	for (GameModel* model : models)
		model->Render();
}

void TestModel::PostRender()
{

}
