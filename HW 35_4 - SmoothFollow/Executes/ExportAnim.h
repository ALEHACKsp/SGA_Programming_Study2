#pragma once
#include "Execute.h"

class ExportAnim : public Execute
{
public:
	ExportAnim(ExecuteValues* values);
	~ExportAnim();

	// Execute을(를) 통해 상속됨
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();
	
private:

};