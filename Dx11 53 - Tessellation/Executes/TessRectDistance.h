#pragma once
#include "Execute.h"

class TessRectDistance : public Execute
{
public:
	TessRectDistance();
	~TessRectDistance();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}
	
private:
	Shader* shader;
	ID3D11Buffer* vertexBuffer;
};