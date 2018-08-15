#pragma once

#include "Line.h"

class Gizmo : public Line
{
public:
	Gizmo(float length = 5.0f);
	~Gizmo();

	void Render(D3DXVECTOR3& startPos,
		D3DXVECTOR3& forward, D3DXVECTOR3& up, D3DXVECTOR3& right);

	void SetLength(float length) { this->length = length; }
private:
	float length;
};