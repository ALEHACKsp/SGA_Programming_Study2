#pragma once
#include "Execute.h"

class TestShadow : public Execute
{
public:
	TestShadow(ExecuteValues* values);
	~TestShadow();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}

private:
	Shader* hanjoShader;
	class GameAnimModel* hanjo;

	Shader* planeShader;
	class MeshPlane* plane;

	class MeshPlane* board;
	Texture* texture;

	//Render2D* render2D;
	RenderTarget* renderTarget;
	class MeshPlane* cctv;
	Texture* texture2;

	class MeshPlane* wall;

	class MeshSphere* sun;
	Shader* sunShader;

	class Shadow* shadow;
};