#pragma once

class MapToolCommand
{
public:
	virtual ~MapToolCommand() {}

	virtual void ChangeData() = 0;
	virtual void Excute() =  0;
	virtual void Undo() = 0;
};