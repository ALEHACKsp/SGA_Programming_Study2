#pragma once
#include "Execute.h"

class TessTriangle : public Execute
{
public:
	TessTriangle();
	~TessTriangle();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}
	
private:
	Shader* shader;
	ID3D11Buffer* vertexBuffer;
};