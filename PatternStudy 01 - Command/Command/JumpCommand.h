#pragma once

#include "Command.h"

class JumpCommand : public Command 
{
public:
	virtual void Execute() { Jump(); }
	virtual void Undo() {}
	virtual void Render() {}

private:
	void Jump() {
		ImGui::SameLine(90);
		ImGui::Text("Jump Command");
		ImGui::SetClipboardText("Jump");
	}
};