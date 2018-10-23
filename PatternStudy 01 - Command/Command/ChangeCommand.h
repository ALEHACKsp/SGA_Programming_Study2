#pragma once

#include "Command.h"

class ChangeCommand : public Command
{
public:
	virtual void Execute() { Change(); }
	virtual void Undo() {}
	virtual void Render() {}

private:
	void Change() {
		ImGui::SameLine(90);
		ImGui::Text("Change Command");
		ImGui::SetClipboardText("Change");
	}
};