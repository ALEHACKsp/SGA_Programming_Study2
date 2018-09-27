#pragma once

#include "Line.h"

class DebugDrawLine : public Line
{
public:
	DebugDrawLine(D3DXVECTOR3& start, D3DXVECTOR3& end);
	~DebugDrawLine();

	void Render();
	void PostRender();
	
	void Get(D3DXVECTOR3* start, D3DXVECTOR3* end)
	{
		start = &this->start;
		end = &this->end;
	}
	void Set(D3DXVECTOR3& start, D3DXVECTOR3& end);
		
private:
	D3DXVECTOR3 start;
	D3DXVECTOR3 end;

	bool selectAxis[3];
	float length;
};