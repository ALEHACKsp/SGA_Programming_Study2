#pragma once

enum Memory_Kind {
	Memory_Create, Memory_Delete,
	Memory_Position, Memory_Rotation, Memory_Scale};

class GameRender;

struct MemoryData
{
	GameRender* target;
	Memory_Kind kind;
	D3DXVECTOR3 val;
};

class Memory
{
public:
	Memory();
	~Memory();

	static void Do(GameRender* target, Memory_Kind kind, D3DXVECTOR3 val = { 0,0,0 });
	static bool UnDo();
	static bool ReDo();

	static void PostRender();

private:
	static stack<MemoryData> undoStack;
	static stack<MemoryData> redoStack;
};