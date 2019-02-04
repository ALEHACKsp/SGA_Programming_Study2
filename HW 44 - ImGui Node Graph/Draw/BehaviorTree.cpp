#include "stdafx.h"
#include "BehaviorTree.h"

#include "Draw/Unit.h"

BehaviorTree::BehaviorTree()
	: show_grid(true), node_selected(-1), scrolling(ImVec2(0,0))
	, link_node(false), bProgress(false)//, runningNode(NULL)
	, deltaTime(0), rate(3.0f)
{
	// root node
	nodes.insert(make_pair(NodeId++, new Node(NodeType::Root, ImVec2(64 * 3 + 16, 64 * 1 + 16))));
}

BehaviorTree::~BehaviorTree()
{
	for (pair<const int, Node*>& p : nodes)
		SAFE_DELETE(p.second);
}

// Node�� vector Child Sort�� ���� �Լ�
bool NodeCmp(const Node* n1, const Node* n2) {
	if (n1->Pos.x < n2->Pos.x) return true;
	else if (n1->Pos.x == n2->Pos.x) return n1->Pos.y < n2->Pos.y;
	else return false;
}

void BehaviorTree::Render()
{
	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Behavior Tree"))
	{
		ImGui::End();
		return;
	}

#pragma region Draw a list of blackboard & behavior tree list on the left side

	bool open_context_menu = false;
	int node_hovered_in_scene = -1;

	ImGui::BeginChild("Blackboard", ImVec2(100, 0));

	ImGui::Checkbox("Progress", &bProgress);

	ImGui::Text("Keys");
	ImGui::Separator();

	for (Blackboard& board : blackboards) {
		string name = board.Name + "##key_list";
		if (board.Keys.size() != 0) {
			if (ImGui::TreeNode(name.c_str())) {
				for (string& key : board.Keys)
					ImGui::BulletText(key.c_str());
				ImGui::TreePop();
			}
		}
	}

	ImGui::Text("Tasks");
	ImGui::Separator();

	for (Blackboard& board : blackboards) {
		string name = board.Name + "##task_list";
		if (board.Tasks.size() != 0) {
			if (ImGui::TreeNode(name.c_str())) {
				for (string& task : board.Tasks)
					ImGui::BulletText(task.c_str());
				ImGui::TreePop();
			}
		}
	}

	ImGui::Text("Services");
	ImGui::Separator();

	for (Blackboard& board : blackboards) {
		string name = board.Name + "##service_list";
		if (board.Services.size() != 0) {
			if (ImGui::TreeNode(name.c_str())) {
				for (string& service : board.Services)
					ImGui::BulletText(service.c_str());
				ImGui::TreePop();
			}
		}
	}

	ImGui::Text("Decorators");
	ImGui::Separator();

	for (Blackboard& board : blackboards) {
		string name = board.Name + "##decorator_list";
		if (board.Decorators.size() != 0) {
			if (ImGui::TreeNode(name.c_str())) {
				for (string& decorator : board.Decorators)
					ImGui::BulletText(decorator.c_str());
				ImGui::TreePop();
			}
		}
	}

	ImGui::EndChild();

#pragma endregion

	ImGui::SameLine();
	ImGui::BeginGroup();

	const float NODE_SLOT_RADIUS = 4.0f;
	const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

#pragma region Draw Canvas

	// Create our child canvas
	ImGui::Checkbox("Grid", &show_grid);

	// Edit Node Color
	for (int i = 0; i < NodeType::End; i++) {
		string label = "##Color" + to_string(i);
		ImGui::SameLine((i + 1) * 25 + 50);
		ImGui::ColorEdit4(label.c_str(), (float*)&NodeColor[i], ImGuiColorEditFlags_NoInputs);
	}

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
	ImGui::BeginChild("scrolling_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
	ImGui::PushItemWidth(120.0f);

	ImVec2 offset = ImGui::GetCursorScreenPos() + scrolling;
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	// Display grid
	if (show_grid)
	{
		ImU32 GRID_COLOR = IM_COL32(200, 200, 200, 40);
		float GRID_SZ = 64.0f;
		ImVec2 win_pos = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetWindowSize();
		for (float x = fmodf(scrolling.x, GRID_SZ); x < canvas_sz.x; x += GRID_SZ)
			draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_sz.y) + win_pos, GRID_COLOR);
		for (float y = fmodf(scrolling.y, GRID_SZ); y < canvas_sz.y; y += GRID_SZ)
			draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_sz.x, y) + win_pos, GRID_COLOR);
	}

	draw_list->ChannelsSplit(2);
	draw_list->ChannelsSetCurrent(0); // Background

	// Display nodes
	for (pair<const int, Node*>& p : nodes)
	{
		Node* node = p.second;
		ImGui::PushID(node->ID);
		ImVec2 node_rect_min = offset + node->Pos;

		// Display node contents first
		draw_list->ChannelsSetCurrent(1); // Foreground
		bool old_any_active = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
		ImGui::BeginGroup(); // Lock horizontal position

		// Display Node's
		ImVec2 item_rect_min, item_rect_max;
		ImVec2 padding = ImVec2(2, 2);

		for (string decorator : node->Decorators) {
			ImGui::Text("%s", decorator.c_str());
			item_rect_min = ImGui::GetItemRectMin() - padding;
			item_rect_max = ImGui::GetItemRectMax() + padding;
			draw_list->AddRectFilled(item_rect_min, item_rect_max, NodeColor[NodeType::Decorator], 1.0f);
			draw_list->AddRect(item_rect_min, item_rect_max, NodeColor[NodeType::Decorator], 1.0f);
		}
		
		ImGui::Text("%s", node->Name.c_str());
		item_rect_min = ImGui::GetItemRectMin() - padding;
		item_rect_max = ImGui::GetItemRectMax() + padding;
		draw_list->AddRectFilled(item_rect_min, item_rect_max, NodeColor[node->Type], 1.0f);
		draw_list->AddRect(item_rect_min, item_rect_max, NodeColor[node->Type], 1.0f);

		for (string service : node->Services) {
			ImGui::Text("%s", service.c_str());
			item_rect_min = ImGui::GetItemRectMin() - padding;
			item_rect_max = ImGui::GetItemRectMax() + padding;
			draw_list->AddRectFilled(item_rect_min, item_rect_max, NodeColor[NodeType::Service], 1.0f);
			draw_list->AddRect(item_rect_min, item_rect_max, NodeColor[NodeType::Service], 1.0f);
		}

		ImGui::EndGroup();

		// Save the size of what we have emitted and whether any of the widgets are being used
		bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
		node->Size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
		ImVec2 node_rect_max = node_rect_min + node->Size;

		// Display Order
		ImVec2 orderPos = ImVec2(node_rect_min.x + node->Size.x, node_rect_min.y);
		ImGui::SetCursorScreenPos(orderPos);
		if (node->Order != -1)
			ImGui::Text("%2d", node->Order);

		// Display node box
		draw_list->ChannelsSetCurrent(0); // Background
		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node->Size);
		if (ImGui::IsItemHovered())
		{
			node_hovered_in_scene = node->ID;
			open_context_menu |= ImGui::IsMouseClicked(1);
		}
		bool node_moving_active = ImGui::IsItemActive();
		if (node_widgets_active || node_moving_active)
			node_selected = node->ID;
		if (node_moving_active && ImGui::IsMouseDragging(0))
			node->Pos = node->Pos + ImGui::GetIO().MouseDelta;

		if (link_node == false && ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)) {
			// �θ� ������ �ڽĵ鳢�� ���� (�������� x ���� ����)
			if (node->Parent != NULL && node->Parent->Childs.size() > 1) {
				Node* parent = node->Parent;
				sort(parent->Childs.begin(), parent->Childs.end(), NodeCmp);

				// �ش� ��尡 ������ ���ԵǾ� ������ �ٽ� ���� ���
				if (node->Order != -1)
					Order();
			}
		}

		//ImColor node_bg_color = (node_hovered_in_list == node->ID || node_hovered_in_scene == node->ID || (node_hovered_in_list == -1 && node_selected == node->ID) || (task_hovered_in_list == task_selected) ) ? 
		//	node->Color + ImVec4(0.1f,0.1,0.1f, 0.0f) : node->Color;
		ImColor node_bg_color = (node_hovered_in_scene == node->ID) ?
			IM_COL32(75, 75, 75, 255) : IM_COL32(60, 60, 60, 255);

		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);

		// Root�� ������ �� ����
		if (node->Type != NodeType::Root)
			draw_list->AddCircleFilled(offset + node->InDegreePos(), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
		// Task������ ������ ���� ����
		if (node->Type != NodeType::Task)
			draw_list->AddCircleFilled(offset + node->OutDegreePos(), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));

		if (node->Order != -1)
			draw_list->AddCircleFilled(orderPos + ImVec2(8.25f, 7), 10, IM_COL32(170, 170, 170, 150));

		ImGui::PopID();
	}

	// link node
	if (link_node && ImGui::IsMouseReleased(0)) {
		link_node = false;
		if (node_hovered_in_scene != -1 && start_node != node_hovered_in_scene) {
			end_node = node_hovered_in_scene;
			Node* start = nodes[start_node];
			Node* end = nodes[end_node];

			LinkNode(start, end);
		}
	}

	// Display links
	for (pair<const int, Node*>& p : nodes)
	{
		Node* node = p.second;

		ImVec2 p1 = offset + node->OutDegreePos();

		if (link_node && node->ID == start_node) {
			ImVec2 p2 = ImGui::GetMousePos();
			draw_list->AddBezierCurve(p1, p1 + ImVec2(0, +50), p2 + ImVec2(0, -50), p2, IM_COL32(200, 200, 100, 255), 3.0f);
		}

		for (Node* child : node->Childs) {
			ImVec2 p2 = offset + child->InDegreePos();
			draw_list->AddBezierCurve(p1, p1 + ImVec2(0, +50), p2 + ImVec2(0, -50), p2, IM_COL32(200, 200, 100, 255), 3.0f);
		}
	}

	draw_list->ChannelsMerge();

	// Open context menu
	if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1))
	{
		//node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
		node_selected = node_hovered_in_scene = -1;
		open_context_menu = true;
	}
	if (open_context_menu)
	{
		ImGui::OpenPopup("context_menu");
		if (node_hovered_in_scene != -1)
			node_selected = node_hovered_in_scene;
	}

	// Draw context menu
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("context_menu"))
	{
		Node* node = node_selected != -1 ? nodes[node_selected] : NULL;
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
		if (node)
		{
			ImGui::Text("Node '%s'", node->Name.c_str());
			ImGui::Separator();
			if (ImGui::MenuItem("Add Link", NULL, false, node->Type != NodeType::Task)) {
				link_node = true;
				start_node = node->ID;
			}
			if (ImGui::MenuItem("Delete Link", "Delete InDegree Link")) {
				// �θ� ���� ����
				Node* parent = node->Parent;
				if (parent != NULL) {
					vector<Node*>::iterator iter = parent->Childs.begin();
					for (; iter != parent->Childs.end(); iter++) {
						if (*iter == node) {
							parent->Childs.erase(iter);
							break;
						}
					}
				}

				// �ڽ� �Ӿƴ϶� �ڽĵ���� �ʱ�ȭ
				ResetOrder(node);
				// �ٽ� ���� ���
				Order();
			}
			if (ImGui::MenuItem("Delete Node", NULL, false, node->Type != NodeType::Root)) {
				Node* parent = node->Parent;
				if (parent != NULL) {
					vector<Node*>::iterator iter = parent->Childs.begin();
					for (; iter != parent->Childs.end(); iter++) {
						if (*iter == node) {
							parent->Childs.erase(iter);
							break;
						}
					}
				}

				for (Node* child : node->Childs) {
					child->Parent = NULL;
				}

				// �ڽ� �Ӿƴ϶� �ڽĵ���� �ʱ�ȭ
				ResetOrder(node);

				map<int, Node*>::iterator iter = nodes.begin();
				for (; iter != nodes.end(); iter++) {
					if (iter->second == node) {
						nodes.erase(iter);
						break;
					}
				}

				// �ٽ� ���� ���
				Order();
			}

			if (ImGui::TreeNode("Decorators")) {

				for (Blackboard board : blackboards) {
					string name = board.Name + "##add_decorators";
					if (board.Tasks.size() != 0) {
						if (ImGui::TreeNode(name.c_str())) {

							for (string decorator : board.Decorators) {
								name = decorator + "##add_decorator";
								// Root�� Decorator �߰� ����
								if (ImGui::MenuItem(name.c_str(), NULL, false, node->Type != NodeType::Root)) {
									node->Decorators.push_back(decorator);
								}
							}

							ImGui::TreePop();
						}
					}
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNode("Services")) {

				for (Blackboard board : blackboards) {
					string name = board.Name + "##add_services";
					if (board.Tasks.size() != 0) {
						if (ImGui::TreeNode(name.c_str())) {

							for (string service : board.Services) {
								name = service + "##add_service";
								// Root�� Service �߰� ����
								if (ImGui::MenuItem(name.c_str(), NULL, false, node->Type != NodeType::Root)) {
									node->Services.push_back(service);
								}
							}

							ImGui::TreePop();
						}
					}
				}

				ImGui::TreePop();
			}
		}
		else
		{
			if (ImGui::TreeNode("Composites")) {

				if (ImGui::MenuItem("Selector", NULL, false)) {
					nodes.insert(make_pair(NodeId++, new Node(NodeType::Composite_Selector, scene_pos)));
				}

				if (ImGui::MenuItem("Sequence", NULL, false)) {
					nodes.insert(make_pair(NodeId++, new Node(NodeType::Composite_Sequence, scene_pos)));
				}

				ImGui::TreePop();
			}
			if (ImGui::TreeNode("Tasks")) {

				// Test
				//if (ImGui::MenuItem("Move", NULL, false)) {
				//	nodes.insert(make_pair(NodeId++, new Node(NodeType::Task, scene_pos, "Move")));
				//}

				for (Blackboard board : blackboards) {
					string name = board.Name + "##add_tasks";
					if (board.Tasks.size() != 0) {
						if (ImGui::TreeNode(name.c_str())) {

							for (string task : board.Tasks) {
								name = task + "##add_task";
								if (ImGui::MenuItem(name.c_str(), NULL, false)) {
									nodes.insert(make_pair(NodeId++, new Node(NodeType::Task, scene_pos, task)));
								}
							}

							ImGui::TreePop();
						}
					}
				}

				ImGui::TreePop();
			}
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();

	// Scrolling
	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
		scrolling = scrolling + ImGui::GetIO().MouseDelta;

#pragma endregion

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::EndGroup();

	ImGui::End();
}

void BehaviorTree::Order()
{
	if (nodes.size() > 0) {
		check.clear();
		for (pair<const int, Node*> p : nodes) 
			check[p.first] = false;
		
		orderNum = 0; // ���� ��ȣ

		DFS(nodes[0]);
	}
}

void BehaviorTree::ResetOrder(Node * node)
{
	node->Order = -1;

	for (Node* child : node->Childs)
		ResetOrder(child);
}

void BehaviorTree::DFS(Node * node)
{
	check[node->ID] = true;

	node->Order = orderNum++;

	for (Node * child : node->Childs) {
		if (check[child->ID] == false) {
			DFS(child);
		}
	}

}

void BehaviorTree::LinkNode(Node * start, Node * end)
{
	// Root�� ������ �� ����
	if (end->Type != NodeType::Root) {
		// �θ� ���� ��� �ٷ� �̾����
		if (end->Parent == NULL) {
			start->Childs.push_back(end);
			end->Parent = start;
		}
		// ������ ���� �̾������
		else {
			Node* oldParent = end->Parent;

			vector<Node*>::iterator iter = oldParent->Childs.begin();
			for (; iter != oldParent->Childs.end(); iter++) {
				if (*iter == end) {
					oldParent->Childs.erase(iter);
					break;
				}
			}

			start->Childs.push_back(end);
			end->Parent = start;
		}

		// ���� ����
		Order();
	}
}

void BehaviorTree::AddBlackboard(IBlackboard * blackboard)
{
	Blackboard board;

	board.Name = blackboard->BlackboardName();
	//blackboard->BlackboardKeys(board.Keys, board.KeyFuncs);
	blackboard->BlackboardTasks(board.Tasks, board.TaskFuncs);
	blackboard->BlackboardServices(board.Services, board.ServiceFuncs);
	blackboard->BlackboardDecorators(board.Decorators, board.DecoratorFuncs);

	blackboards.push_back(board);
}

void BehaviorTree::Init()
{
	Node* start = nodes[0];
	
	nodes.insert(make_pair(NodeId++, new Node(NodeType::Composite_Sequence, ImVec2(64 * 3 + 16, 64 * 2 + 16))));

	Node* end = nodes[NodeId - 1];

	LinkNode(start, end);

	start = end;

	for (Blackboard board : blackboards) {
		int i = 0;
		for (string task : board.Tasks) {
			nodes.insert(make_pair(NodeId++, new Node(NodeType::Task, ImVec2((64 + 64 + 32 + 16) * i++ + 16, 64 * 3 + 16), task)));
			end = nodes[NodeId - 1];

			LinkNode(start, end);

			if (task == "MoveToVendingMachine")
				end->Decorators.push_back("IsMoneyEnough");
		}
	}

}

void BehaviorTree::Update()
{
	if (bProgress == false) return;

	deltaTime += Time::Delta();

	if (deltaTime >= rate) {
		deltaTime = 0;

		// �׻� root���� ����
		Progress(nodes[0]);

		//// runningNode�� ���� �ʰ�(NULL�̸�) stk�� ��� ������ Root Node ���� ����
		//if (runningNode == NULL) 
		//	Progress(nodes[0]);
		//else {
		//	bool result = Progress(runningNode);

		//	// running �� ���� ���
		//	if (result == true) {
		//		Node* temp = stk.top();
		//		// stack���� ���� Node�� root�� ��� runningNode�� null �ֱ�
		//		if(temp->Type == NodeType::Root)
		//			runningNode = NULL;
		//		else if (temp->Type == NodeType::Composite_Selector) {
		//			// true�� ��� selector true�� ��
		//			if (result == true) {

		//			}
		//			// false�� ��� ���� �ڽ� or selector false�� ��
		//			else {

		//			}
		//		}
		//	}
		//}

	}
}

bool BehaviorTree::Progress(Node * node)
{
	//stk.push(node);

	// Decorator
	for (string& decorator : node->Decorators) {
		for (Blackboard& board : blackboards) {
			if (board.DecoratorFuncs.find(decorator) != board.DecoratorFuncs.end()) {
				bool result = board.DecoratorFuncs[decorator]();
				// decorator�� ��ȯ���� false�� false return�Ͽ� �ٽ� root���� ����
				if (result == false) return false;
				break;
			}
		}
	}

	// Service
	for (string& service : node->Services) {
		for (Blackboard& board : blackboards) {
			if (board.ServiceFuncs.find(service) != board.ServiceFuncs.end()) {
				board.ServiceFuncs[service]();
				break;
			}
		}
	}

	// Composite or Task

	bool result = true;

	switch (node->Type)
	{
		case Root: 
		{
			// ��Ʈ�� �׳� �ڽ� �����ؼ� ������� �ޱ� false������ üũ 
			// ������� false�� ���� Update������ root���� �ٽ� ������
			for (Node* child : node->Childs) {
				result &= Progress(child);

				//stk.pop();
			}
			break;
		}
		case Composite_Selector:
		{
			bool check = false;
			// �ڽ� �� �ϳ��� ���࿡ �����ϸ� �ڼ��� ���� ���� 
			// �ϳ��� ���� �����ϸ� Selector ����
			// ��� �ڼ� ���� �����ϸ� ����
			for (Node* child : node->Childs) {
				bool temp = Progress(child);

				//// �ڽ��� ������ running ���� �ִ� ��� break
				//if (runningNode != NULL) {
				//	runningCheck = check;
				//	break;
				//}

				check |= temp;

				//stk.pop();

				if (check == true)
					break;
			}
			result = check;
			break;
		}
		case Composite_Sequence:
		{
			bool check = true;
			// �ڽ� �� �ϳ��� ���࿡ �����ϸ� ���� ����
			// ��� �ڼ� ������ �����ؾ� Sequence ����
			// �ڼ� �ϳ��� �����ϸ� ����
			for (Node* child : node->Childs) {
				bool temp = Progress(child);

				//// �ڽ��� ������ running ���� �ִ� ��� break
				//if (runningNode != NULL) {
				//	runningCheck = check;
				//	break;
				//}

				check &= temp;

				//stk.pop();

				if (check == false)
					break;
			}
			result = check;
			break;
		}
		case Task:
		{
			string task = node->Name;
			// �����忡�� task ã�Ƽ� ����
			for (Blackboard& board : blackboards) {
				if (board.TaskFuncs.find(task) != board.TaskFuncs.end()) {
					TaskResult taskResult = board.TaskFuncs[task]();
					// task�� ������ ���� ������ ������ ���� Update�� �ش� task���� �ٽ� ���� (runningNode�� ���)
					if (taskResult == TaskResult::Result_Running) {
						//runningNode = node;

						// �θ��� composite Ÿ�Կ� ���� running�� ���ӵǰ� (���� �ڽ����� ���� �ʰ�) ����
						if (node->Parent->Type == NodeType::Composite_Selector)
							result = true;
						else if (node->Parent->Type == NodeType::Composite_Sequence)
							result = false;
					}
					// running�� ������ runningNode null�� ����
					//else
						//runningNode = NULL;
					else
						result = taskResult;

					break;
				}
			}
			break;
		}
	}

	return result;
}
