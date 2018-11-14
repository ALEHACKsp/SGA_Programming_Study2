#include "stdafx.h"
#include "GizmoAxis.h"

GizmoAxis::GizmoAxis(Kind kind, float lineOffset, float halfLineOffset, float lineLength)
	:kind(kind),
	halfLineOffset(halfLineOffset), lineLength(lineLength), lineOffset(lineOffset)
{
	switch (kind)
	{
	case GizmoAxis::Kind::X:
	{
		lines.push_back(D3DXVECTOR3(halfLineOffset, 0, 0));
		lines.push_back(D3DXVECTOR3(lineLength, 0, 0));

		lines.push_back(D3DXVECTOR3(lineOffset, 0, 0));
		lines.push_back(D3DXVECTOR3(lineOffset, lineOffset, 0));

		lines.push_back(D3DXVECTOR3(lineOffset, 0, 0));
		lines.push_back(D3DXVECTOR3(lineOffset, 0, lineOffset));
	}
		break;
	case GizmoAxis::Kind::Y:
	{
		lines.push_back(D3DXVECTOR3(0, halfLineOffset, 0));
		lines.push_back(D3DXVECTOR3(0, lineLength, 0));

		lines.push_back(D3DXVECTOR3(0, lineOffset, 0));
		lines.push_back(D3DXVECTOR3(lineOffset, lineOffset, 0));

		lines.push_back(D3DXVECTOR3(0, lineOffset, 0));
		lines.push_back(D3DXVECTOR3(0, lineOffset, lineOffset));
	}
		break;
	case GizmoAxis::Kind::Z:
	{
		lines.push_back(D3DXVECTOR3(0, 0, halfLineOffset));
		lines.push_back(D3DXVECTOR3(0, 0, lineLength));

		lines.push_back(D3DXVECTOR3(0, 0, lineOffset));
		lines.push_back(D3DXVECTOR3(lineOffset, 0, lineOffset));

		lines.push_back(D3DXVECTOR3(0, 0, lineOffset));
		lines.push_back(D3DXVECTOR3(0, lineOffset, lineOffset));
	}
		break;
	}
}

void GizmoAxis::GetLine(D3DXMATRIX & world, vector<D3DXVECTOR3>& lines)
{
	for (int i = 0; i < this->lines.size(); i++) {
		D3DXVECTOR3 temp;
		D3DXVec3TransformCoord(&temp, &this->lines[i], &world);

		lines.push_back(temp);
	}
}
