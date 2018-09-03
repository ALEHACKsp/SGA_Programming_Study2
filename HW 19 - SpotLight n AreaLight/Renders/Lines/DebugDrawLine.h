#pragma once

#include "Line.h"

class DebugDrawLine : public Line
{
public:
	DebugDrawLine(D3DXVECTOR3& start, D3DXVECTOR3& end);
	~DebugDrawLine();

	void Render();
	void PostRender();
	
	void GetPosition(D3DXVECTOR3* start, D3DXVECTOR3* end)
	{
		start = &this->start;
		end = &this->end;
	}
	void SetPosition(D3DXVECTOR3& start, D3DXVECTOR3& end);
		
private:
	D3DXVECTOR3 start;
	D3DXVECTOR3 end;

	bool selectAxis[3];
	float length;
};