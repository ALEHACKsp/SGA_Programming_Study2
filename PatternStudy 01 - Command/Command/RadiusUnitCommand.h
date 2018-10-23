#pragma once

#include "Command.h"
#include "../Objects/Unit.h"

class RadiusUnitCommand : public Command
{
public:
	RadiusUnitCommand(Unit* unit, float radius)
		: unit(unit), radius(radius) {}

	virtual void Execute() {
		radiusBefore = unit->GetRadius();
		unit->SetRadius(radius);
	}

	virtual void Undo() {
		unit->SetRadius(radiusBefore);
	}

	virtual void Render() {
		ImGui::Text((unit->GetName() + "'s Radius Command : Radius %.2f").c_str(),
			radius);
	}


private:
	Unit* unit;
	float radius;
	float radiusBefore;
};