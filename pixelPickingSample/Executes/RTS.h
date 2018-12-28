#pragma once
#include "Execute.h"

class RTS : public Execute
{
public:
	RTS(ExecuteValues* values);
	~RTS();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen() {}

private:
	void SwatCommend();

private:
	class Sky* sky;

	vector<GameModel*> monsters;

	class Swat* swat;

	D3DXVECTOR3 camPos;
	float rotationSpeed = 2.5f;
	float moveSpeed = 20.0f;
	bool forward, backward, left, right;
};