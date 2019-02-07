#pragma once

#include "IBlackboard.h"

enum NodeType {
	Root, // 비헤이비어 트리 시작점, 단 하나의 연결만, Decorator, Service 못 붙임
	// Composite : 해당 분기가 실행되는 기본 규칙 정의
	// Decorator를 통해서 분기로 들어가는 조건 변경, 중간에 실행 취소, Service를 덧붙여 Composite Node의 Child가 실행되는 동안 Service 작동 가능
	Composite_Selector, // Child Node 중 하나가 실행에 성공하면 Child의 실행을 멈춤, 
	// Selector의 Child가 실행에 성공하면 Selector의 작동 성공, 모든 Child의 실행 실패 Selector 실패
	Composite_Sequence, // Child Node 중 하나가 실패하면 Child의 실행 멈춤,
	// Child Node가 실행에 실패하면 Sequecne 실패, 모든 Child Node가 실행에 성공해야 Sequecne 성공
	Task, // AI의 이동이나 Blackboard의 값 조정과 같은 작업을 하는 Node, 이 Node에도 Decorator가 붙을 수 있음
	Decorator, // *Node는 아님, Composite Node에 분기가 실행되는 동안 
	// 정해진 빈도에 맞춰서 실행, 보통 검사를 하고 그 검사를 바탕으로 블랙보드의 내용을 업데이트하는데 사용
	Service, // *Node는 아님, 조건절이라고도 부르는 것으로, Composite나 Task에 붙여서
	// 분기나 노드가 실행될 것인지를 정의
	End
};

static int NodeId = 0;

static ImColor NodeColor[] = {
	ImColor(0.4f, 0.4f, 0.4f, 0.5f), // Root Color
	ImColor(0.5f, 0.5f, 0.5f, 0.5f), // Composite_Selector Color
	ImColor(0.5f, 0.5f, 0.5f, 0.5f), // Composite_Sequence Color
	ImColor(0.5f, 0.1f, 0.7f, 0.5f), // Task Color
	ImColor(0.0f, 0.0f, 0.7f, 0.5f), // Decorator Color
	ImColor(0.0f, 0.7f, 0.0f, 0.5f), // Service Color
};

enum NodeStatus { Status_False, Status_True, Status_Running };

struct Node
{
	int ID;
	string Name;
	NodeType Type;
	ImVec2 Pos, Size;
	ImColor& Color;
	int Order; // 실행 순서

	NodeStatus Status;

	Node* Parent;
	vector<Node*> Childs;

	vector<string> Decorators;
	vector<string> Services;

	Node(NodeType type, const ImVec2& pos, string name = "") 
		: Type(type), Pos(pos), ID(NodeId), Parent(NULL), Order(-1)
		, Color(NodeColor[type]), Status(Status_False)
	{
	
		switch (type)
		{
		case Root: Name = "Root"; Order = 0; break;
		case Composite_Selector: Name = "Selector"; break;
		case Composite_Sequence: Name = "Sequence"; Status = Status_True; break;
		case Task: Name = name; break;
		}
	}

	string Label() { return Name + "##" + to_string(ID); }

	// InDegree 입력 차수 : 한 꼭짓점으로 들어오는 변의 개수
	ImVec2 InDegreePos() { return ImVec2(Pos.x + Size.x / 2, Pos.y); }
	// OutDegree 출력 차수 : 한 꼭짓점에서 나가는 변의 개수
	ImVec2 OutDegreePos() { return ImVec2(Pos.x + Size.x / 2, Pos.y + Size.y); }

};

class BehaviorTree
{
public:
	BehaviorTree();
	~BehaviorTree();
	
	void Update();
	void Render();

	void AddBlackboard(IBlackboard* blackboard);
	// blackboard가 다 들어오고 나서 init 시작해야함
	void Init();

	bool Progress(Node* node);

private:
	void Order();
	void Reset(bool bRoot = true);
	void ResetOrder(Node* node);
	void ResetStatus(Node* node = NULL);
	void DFS(Node* node);
	void LinkNode(Node* start, Node* end);

	void Save(wstring fileName);
	void Load(wstring fileName);

	Node* Find(int id);

private:
	map<int, Node*> nodes;
	ImVec2 scrolling;
	bool show_grid;
	int node_selected;

	bool link_node;
	int start_node, end_node;

	bool bProgress;
	bool bRunning;

private:
	int orderNum; // 순서
	map<int, bool> check;

	float deltaTime;
	float rate;

	wstring saveFolder;
	JsonHelper json;

private:
	struct Blackboard {
		string Name;

		vector<string> Tasks;
		map<string, function<TaskResult()> > TaskFuncs;
		map<string, string> TaskTips;

		// key가 조금 애매함
		//vector<string> Keys;
		//map<string, function<void(OUT D3DXVECTOR3)> > KeyFuncs;
		//map<string, string> KeyTips;

		vector<string> Services;
		map<string, function<void()> > ServiceFuncs;
		map<string, string> ServicesTips;

		vector<string> Decorators;
		map<string, function<bool()> > DecoratorFuncs;
		map<string, string> DecoratorsTips;
	};

	vector<Blackboard> blackboards;
};
