#pragma once

enum NodeType {
	Root, // �����̺�� Ʈ�� ������, �� �ϳ��� ���Ḹ, Decorator, Service �� ����
	// Composite : �ش� �бⰡ ����Ǵ� �⺻ ��Ģ ����
	// Decorator�� ���ؼ� �б�� ���� ���� ����, �߰��� ���� ���, Service�� ���ٿ� Composite Node�� Child�� ����Ǵ� ���� Service �۵� ����
	Composite_Selector, // Child Node �� �ϳ��� ���࿡ �����ϸ� Child�� ������ ����, 
	// Selector�� Child�� ���࿡ �����ϸ� Selector�� �۵� ����, ��� Child�� ���� ���� Selector ����
	Composite_Sequence, // Child Node �� �ϳ��� �����ϸ� Child�� ���� ����,
	// Child Node�� ���࿡ �����ϸ� Sequecne ����, ��� Child Node�� ���࿡ �����ؾ� Sequecne ����
	Task, // AI�� �̵��̳� Blackboard�� �� ������ ���� �۾��� �ϴ� Node, �� Node���� Decorator�� ���� �� ����
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
	int Order; // ���� ����

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

	// InDegree �Է� ���� : �� ���������� ������ ���� ����
	ImVec2 InDegreePos() { return ImVec2(Pos.x + Size.x / 2, Pos.y); }
	// OutDegree ��� ���� : �� ���������� ������ ���� ����
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
