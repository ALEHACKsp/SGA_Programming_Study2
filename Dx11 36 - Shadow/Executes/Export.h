#pragma once
#include "Execute.h"

class Export : public Execute
{
public:
	Export(ExecuteValues* values);
	~Export();

	// Execute��(��) ���� ��ӵ�
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();
	
private:

};