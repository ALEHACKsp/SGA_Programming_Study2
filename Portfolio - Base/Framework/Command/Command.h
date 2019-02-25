#pragma once

static int commandId = 0;

class Command
{
public:
	virtual ~Command() {};

	virtual void Execute() = 0;
	virtual void Undo() = 0;
	virtual void Render() = 0;
};