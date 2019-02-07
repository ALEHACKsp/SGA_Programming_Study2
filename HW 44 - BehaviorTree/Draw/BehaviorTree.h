#pragma once

#include "IBlackboard.h"

enum NodeType {
	Root, // �����̺�� Ʈ�� ������, �� �ϳ��� ���Ḹ, Decorator, Service �� ����
	// Composite : �ش� �бⰡ ����Ǵ� �⺻ ��Ģ ����
	// Decorator�� ���ؼ� �б�� ���� ���� ����, �߰��� ���� ���, Service�� ���ٿ� Composite Node�� Child�� ����Ǵ� ���� Service �۵� ����
	Composite_Selector, // Child Node �� �ϳ��� ���࿡ �����ϸ� Child�� ������ ����, 
	// Selector�� Child�� ���࿡ �����ϸ� Selector�� �۵� ����, ��� Child�� ���� ���� Selector ����
	Composite_Sequence, // Child Node �� �ϳ��� �����ϸ� Child�� ���� ����,
	// Child Node�� ���࿡ �����ϸ� Sequecne ����, ��� Child Node�� ���࿡ �����ؾ� Sequecne ����
	Task, // AI�� �̵��̳� Blackboard�� �� ������ ���� �۾��� �ϴ� Node, �� Node���� Decorator�� ���� �� ����
	Decorator, // *Node�� �ƴ�, Composite Node�� �бⰡ ����Ǵ� ���� 
	// ������ �󵵿� ���缭 ����, ���� �˻縦 �ϰ� �� �˻縦 �������� �������� ������ ������Ʈ�ϴµ� ���
	Service, // *Node�� �ƴ�, �������̶�� �θ��� ������, Composite�� Task�� �ٿ���
	// �б⳪ ��尡 ����� �������� ����
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
	int Order; // ���� ����

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

	// InDegree �Է� ���� : �� ���������� ������ ���� ����
	ImVec2 InDegreePos() { return ImVec2(Pos.x + Size.x / 2, Pos.y); }
	// OutDegree ��� ���� : �� ���������� ������ ���� ����
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
	// blackboard�� �� ������ ���� init �����ؾ���
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
	int orderNum; // ����
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

		// key�� ���� �ָ���
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
