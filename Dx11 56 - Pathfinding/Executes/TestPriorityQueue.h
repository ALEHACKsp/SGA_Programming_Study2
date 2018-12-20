#pragma once
#include "Execute.h"

#include "Utilities\PriorityQueue.h"

class QueueTest : public IPriorityNode
{
public:
	// IPriorityNode을(를) 통해 상속됨
	virtual float Priority() override { return priority; }
	virtual void Priority(float val) override { priority = val; }
	virtual int InsertionIndex() override { return insertionIndex; }
	virtual void InsertionIndex(int val) override { insertionIndex = val; }
	virtual int QueueIndex() override { return queueIndex; }
	virtual void QueueIndex(int val) override { queueIndex = val; }

private:
	float priority = 0;
	int insertionIndex = 0;
	int queueIndex = 0;
};

class TestPriorityQueue : public Execute
{
public:
	TestPriorityQueue();
	~TestPriorityQueue();

	void Update();
	void PreRender();
	void Render();
	void PostRender();
	void ResizeScreen() {}
	
private:

};