#include "stdafx.h"
#include "TestPriorityQueue.h"


TestPriorityQueue::TestPriorityQueue()
{
	int count = 1000;

	PriorityQueue* queue = new PriorityQueue(count);

	for (int i = 0; i < count; i++)
	{
		QueueTest* test = new QueueTest();

		queue->Enqueue(test, Math::Random(0, 1000));
	}

	vector<QueueTest* > q;
	for (int i = 0; i < count; i++)
	{
		q.push_back((QueueTest*)queue->Dequeue());
	}

	int a = 10;

	for (int i = 0; i < q.size(); i++)
		SAFE_DELETE(q[i]);

	SAFE_DELETE(queue);
}

TestPriorityQueue::~TestPriorityQueue()
{

}

void TestPriorityQueue::Update()
{

}

void TestPriorityQueue::PreRender()
{

}

void TestPriorityQueue::Render()
{

}

void TestPriorityQueue::PostRender()
{

}