#include "stdafx.h"
#include "MapToolCommandCaretaker.h"
#include "MapToolCommands.h"

MapToolCommandCaretaker::MapToolCommandCaretaker()
	: command(NULL)
{
}

MapToolCommandCaretaker::~MapToolCommandCaretaker()
{
	SAFE_DELETE(command);
}

bool MapToolCommandCaretaker::IsUndoStack()
{
	return !vUndoStack.empty();
}

bool MapToolCommandCaretaker::IsRedoStack()
{
	return !vRedoStack.empty();
}

void MapToolCommandCaretaker::MakeUndoCommand(MapToolCommand * command)
{
	this->command = command;
}

void MapToolCommandCaretaker::PushUndoCommand(bool redoClear)
{
	command->ChangeData();
	vUndoStack.push_back(command);
	//재실행자료는 삭제
	if (redoClear && !vRedoStack.empty()) {
		for (MapToolCommand* redocommand : vRedoStack) {
			SAFE_DELETE(redocommand);
		}
		vRedoStack.clear();
	}
}

void MapToolCommandCaretaker::PushRedoCommand(MapToolCommand * command)
{
	vRedoStack.push_back(command);
}

void MapToolCommandCaretaker::UndoCommand()
{
	if (IsUndoStack())
	{
		//Undo
		iter = vUndoStack.end() - 1;
		command = (*iter);
		command->Undo();

		//MoveTo RedoStack
		vUndoStack.erase(iter);
		PushRedoCommand(command);
	}
}

void MapToolCommandCaretaker::RedoCommand()
{
	if (IsRedoStack())
	{
		//Excute
		iter = vRedoStack.end() - 1;
		command = (*iter);
		command->Excute();

		//MoveTo UndoStack
		vRedoStack.erase(iter);
		PushUndoCommand(false);
	}
}