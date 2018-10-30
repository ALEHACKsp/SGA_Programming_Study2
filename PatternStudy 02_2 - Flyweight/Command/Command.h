#pragma once

#include "stdafx.h"

class Command
{
public:
	virtual ~Command() {}
	virtual void Execute() = 0;
	virtual void Undo() = 0;

	virtual void Render() = 0;
};
