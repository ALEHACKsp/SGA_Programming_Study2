#pragma once

#include "Command.h"

class BillboardCommand : public Command
{
public:
	BillboardCommand(class Billboard* billboard);
	~BillboardCommand();

	// Command을(를) 통해 상속됨
	virtual void Execute() override;
	virtual void Undo() override;
	virtual void Render() override;
	
private:
	class Billboard* billboard;

	int prevInstanceCount;
	int instanceCount;

	int id;
};
