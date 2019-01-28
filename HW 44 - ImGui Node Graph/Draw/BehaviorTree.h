#pragma once

enum NodeType {
	Root, // 비헤이비어 트리 시작점, 단 하나의 연결만, Decorator, Service 못 붙임
	// Composite : 해당 분기가 실행되는 기본 규칙 정의
	// Decorator를 통해서 분기로 들어가는 조건 변경, 중간에 실행 취소, Service를 덧붙여 Composite Node의 Child가 실행되는 동안 Service 작동 가능
	Composite_Selector, // Child Node 중 하나가 실행에 성공하면 Child의 실행을 멈춤, 
	// Selector의 Child가 실행에 성공하면 Selector의 작동 성공, 모든 Child의 실행 실패 Selector 실패
	Composite_Sequence, // Child Node 중 하나가 실패하면 Child의 실행 멈춤,
	// Child Node가 실행에 실패하면 Sequecne 실패, 모든 Child Node가 실행에 성공해야 Sequecne 성공
	Task, // AI의 이동이나 Blackboard의 값 조정과 같은 작업을 하는 Node, 이 Node에도 Decorator가 붙을 수 있음
	End
};

static int NodeId = 0;

ImColor NodeColor[] = {
	ImColor(0.4f, 0.4f, 0.4f), // Root Color
	ImColor(0.5f, 0.5f, 0.5f), // Composite_Selector Color
	ImColor(0.5f, 0.5f, 0.5f), // Composite_Sequence Color
	ImColor(0.5f, 0.1f, 0.7f), // Task Color
};

struct Node
{
	int ID;
	string Name;
	NodeType Type;
	ImVec2 Pos, Size;
	ImColor& Color;
	int Order; // 실행 순서

	Node* Parent;
	vector<Node*> Childs;

	Node(NodeType type, const ImVec2& pos, string name = "") 
		: Type(type), Pos(pos), ID(NodeId), Parent(NULL), Order(-1)
		, Color(NodeColor[type])
	{
		switch (type)
		{
		case Root: Name = "Root"; Order = 0; break;
		case Composite_Selector: Name = "Selector"; break;
		case Composite_Sequence: Name = "Sequence"; break;
		case Task: Name = name == "" ? "Task" : name; break;
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
	BehaviorTree(bool inited = true);
	~BehaviorTree();

	void Render();

private:
	void Order();
	void DFS(Node* node, int order);

private:
	map<int, Node*> nodes;
	ImVec2 scrolling;
	bool show_grid;
	int node_selected;

	bool link_node;
	int start_node, end_node;

private:
	map<int, bool> check;
};
