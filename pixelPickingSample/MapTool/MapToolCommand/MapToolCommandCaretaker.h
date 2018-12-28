#pragma once

class MapToolCommand;
class MapToolCommandCaretaker
{
public:
	MapToolCommandCaretaker();
	~MapToolCommandCaretaker();
	
	//Ŀ�ǵ带 ������� ���ÿ� ���� ( ������ҿ� �ڷ� ���� )
	void MakeUndoCommand(MapToolCommand * commend);
	void PushUndoCommand(bool redoClear = true);
	
	//Ŀ�ǵ带 ������� ���ÿ��� ���� ( ������� )
	void UndoCommand();

	//Ŀ�ǵ带 ����� ���ÿ��� ���� ( ����� )
	void RedoCommand();

	//���ÿ� �ڷᰡ �����ִ��� Ȯ���ϴ� �Լ�
	bool IsUndoStack();
	bool IsRedoStack();

private:
	//Ŀ�ǵ带 ����� ���ÿ� ����
	void PushRedoCommand(MapToolCommand * commend);

private:
	vector<MapToolCommand*> vUndoStack;
	vector<MapToolCommand*> vRedoStack;
	vector<MapToolCommand*>::iterator iter;

	MapToolCommand* command;
};
