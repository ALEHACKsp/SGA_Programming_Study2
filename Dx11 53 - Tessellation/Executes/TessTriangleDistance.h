#pragma once
#include "Execute.h"

class TessTriangleDistance : public Execute
{
public:
	TessTriangleDistance();
	~TessTriangleDistance();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}
	
private:
	Shader* shader;
	ID3D11Buffer* vertexBuffer;
};