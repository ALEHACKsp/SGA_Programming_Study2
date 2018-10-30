#pragma once

#include "stdafx.h"

class Unit
{
public:
	Unit() 
		: posX(0), posY(0), isShow(true), radius(10), color(D3DXCOLOR(1, 1, 1, 1)) {}
	Unit(float x, float y) 
		: posX(x), posY(y), isShow(true), radius(10), color(D3DXCOLOR(1, 1, 1, 1)) {}

	void SetName(string name) { this->name = name; }
	string GetName() { return name; }

	void SetRadius(float radius) { this->radius = radius; }
	float GetRadius() { return radius; }

	void SetColor(D3DXCOLOR color) { this->color = color; }
	D3DXCOLOR GetColor() { return color; }

	void SetIsShow(bool isShow) { this->isShow = isShow; }
	bool GetIsShow() { return isShow; }

	void MoveTo(float x, float y) {
		posX = x;
		posY = y;
	}

	ImVec2 Pos() { return ImVec2(posX, posY); }

private:
	float posX, posY;
	bool isShow;

	D3DXCOLOR color;
	float radius;

	string name;
};