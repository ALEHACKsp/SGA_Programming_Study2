#pragma once
#include "Execute.h"

class ExportAnim : public Execute
{
public:
	ExportAnim(ExecuteValues* values);
	~ExportAnim();

	// Execute��(��) ���� ��ӵ�
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();
	
private:

};