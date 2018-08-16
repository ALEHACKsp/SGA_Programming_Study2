#pragma once
#include "Execute.h"

class ExeHomework : public Execute
{
public:
	ExeHomework(ExecuteValues* values);
	~ExeHomework();

	// Execute��(��) ���� ��ӵ�
	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen();

private:
	vector<class GameModel*> models;

	int selectModel;
};