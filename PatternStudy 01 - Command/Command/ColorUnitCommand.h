#pragma once

#include "Command.h"
#include "../Objects/Unit.h"

class ColorUnitCommand : public Command
{
public:
	ColorUnitCommand(Unit* unit, D3DXCOLOR color)
		: unit(unit), color(color) {}

	virtual void Execute() {
		colorBefore = unit->GetColor();
		unit->SetColor(color);
	}

	virtual void Undo() {
		unit->SetColor(colorBefore);
	}

	virtual void Render() {
		ImGui::Text((unit->GetName() + "'s Color Command ").c_str());
		ImGui::SameLine(150);
		ImGui::TextColored(ImVec4(color.r, color.g, color.b, color.a), "Color");
	}


private:
	Unit* unit;
	D3DXCOLOR color;
	D3DXCOLOR colorBefore;
};