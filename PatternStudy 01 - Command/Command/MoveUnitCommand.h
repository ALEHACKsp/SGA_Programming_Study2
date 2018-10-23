#pragma once

#include "Command.h"
#include "../Objects/Unit.h"

class MoveUnitCommand : public Command
{
public:
	MoveUnitCommand(Unit* unit, float x, float y)
	: unit(unit), x(x), y(y), xBefore(0), yBefore(0) {}

	virtual void Execute() {
		ImVec2 pos = unit->Pos();

		// ���߿� �̵��� ����� �� �ֵ��� ���� ���� ��ġ�� ����
		xBefore = pos.x;
		yBefore = pos.y;

		unit->MoveTo(x, y);
	}

	virtual void Undo() {
		unit->MoveTo(xBefore, yBefore);
	}

	virtual void Render() {
		ImGui::Text((unit->GetName() + "'s Move Command : Pos %.2f, %.2f").c_str(), 
			x, y);
	}


private:
	Unit* unit;
	float x, y;
	float xBefore, yBefore;
};