#pragma once

#include "Command.h"
#include "../Objects/Unit.h"

class ShowUnitCommand : public Command
{
public:
	ShowUnitCommand(Unit* unit, bool isShow)
		: unit(unit), isShow(isShow) {}

	virtual void Execute() {
		isShowBefore = unit->GetIsShow();
		unit->SetIsShow(isShow);
	}

	virtual void Undo() {
		unit->SetIsShow(isShowBefore);
	}

	virtual void Render() {
		bool isShow = unit->GetIsShow();
		string show = isShow == true ? "Show" : "UnShow";
		ImGui::Text((unit->GetName() + "'s Show Command : " + show).c_str());
	}


private:
	Unit* unit;
	float isShow;
	float isShowBefore;
};