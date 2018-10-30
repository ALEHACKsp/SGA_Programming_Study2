#pragma once

#include "Command.h"

class MoveCommand : public Command
{
public:
	virtual void Execute() { Move(); }
	virtual void Undo() {}
	virtual void Render() {}

private:
	void Move() {
		ImGui::SameLine(90);
		ImGui::Text("Move Command");
		ImGui::SetClipboardText("Move");
	}
};