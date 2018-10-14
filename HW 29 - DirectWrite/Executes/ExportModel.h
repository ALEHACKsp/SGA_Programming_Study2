#pragma once
#include "Execute.h"

class ExportModel : public Execute
{
public:
	ExportModel(ExecuteValues* values);
	~ExportModel();

	// Execute을(를) 통해 상속됨
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void Render2D() {}
	void ResizeScreen();
	
private:

};