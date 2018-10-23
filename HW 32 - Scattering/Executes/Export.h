#pragma once
#include "Execute.h"

class Export : public Execute
{
public:
	Export(ExecuteValues* values);
	~Export();

	// Execute을(를) 통해 상속됨
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();
	
private:

};