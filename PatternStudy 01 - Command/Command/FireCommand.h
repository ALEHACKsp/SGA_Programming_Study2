#pragma once

#include "Command.h"

class FireCommand : public Command {
public:
	virtual void Execute() { Fire(); }
	virtual void Undo() {}
	virtual void Render() {}

private:
	void Fire() {
		ImGui::SameLine(90);
		ImGui::Text("Fire Command");
		ImGui::SetClipboardText("Fire");
	}
};