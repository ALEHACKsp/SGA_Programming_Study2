#pragma once

#include "./Draw/ILine.h"

class GizmoAxis : public ILine
{
public: 
	enum class Kind { X, Y, Z, XZ, XY, YZ };
public:
	GizmoAxis(Kind kind, float lineOffset = 1.0f, float halfLineOffset = 0.5f, float lineLength = 3.0f);

	void GetLine(D3DXMATRIX& world, vector<D3DXVECTOR3>& lines) override;

private:
	float lineOffset;
	float halfLineOffset;
	float lineLength;

	Kind kind;
	vector<D3DXVECTOR3> lines;
};