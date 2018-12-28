#pragma once

class MapToolCommand;
class MapToolCommandCaretaker
{
public:
	MapToolCommandCaretaker();
	~MapToolCommandCaretaker();
	
	//커맨드를 실행취소 스택에 저장 ( 실행취소용 자료 저장 )
	void MakeUndoCommand(MapToolCommand * commend);
	void PushUndoCommand(bool redoClear = true);
	
	//커맨드를 실행취소 스택에서 꺼냄 ( 실행취소 )
	void UndoCommand();

	//커맨드를 재실행 스택에서 꺼냄 ( 재실행 )
	void RedoCommand();

	//스택에 자료가 남아있는지 확인하는 함수
	bool IsUndoStack();
	bool IsRedoStack();

private:
	//커맨드를 재실행 스택에 저장
	void PushRedoCommand(MapToolCommand * commend);

private:
	vector<MapToolCommand*> vUndoStack;
	vector<MapToolCommand*> vRedoStack;
	vector<MapToolCommand*>::iterator iter;

	MapToolCommand* command;
};
