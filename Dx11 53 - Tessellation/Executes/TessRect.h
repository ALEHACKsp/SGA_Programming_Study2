#pragma once
#include "Execute.h"

class TessRect : public Execute
{
public:
	TessRect();
	~TessRect();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}
	
private:
	Shader* shader;
	ID3D11Buffer* vertexBuffer;
};