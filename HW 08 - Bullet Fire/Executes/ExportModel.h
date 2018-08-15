#pragma once
#include "Execute.h"

class ExportModel : public Execute
{
public:
	ExportModel(ExecuteValues* values);
	~ExportModel();

	// Execute��(��) ���� ��ӵ�
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();
	
private:
	class GameModel* tank;
};