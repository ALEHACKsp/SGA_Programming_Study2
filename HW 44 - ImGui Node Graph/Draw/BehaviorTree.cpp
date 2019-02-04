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

// Node의 vector Child Sort를 위한 함수
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
			// 부모가 있으면 자식들끼리 정렬 (포지션의 x 값에 따라)
			if (node->Parent != NULL && node->Parent->Childs.size() > 1) {
				Node* parent = node->Parent;
				sort(parent->Childs.begin(), parent->Childs.end(), NodeCmp);

				// 해당 노드가 순서에 포함되어 있으면 다시 순서 계산
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

		// Root는 들어오는 값 없음
		if (node->Type != NodeType::Root)
			draw_list->AddCircleFilled(offset + node->InDegreePos(), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
		// Task에서는 나가는 값은 없음
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
				// 부모 관계 끊기
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

				// 자신 뿐아니라 자식들까지 초기화
				ResetOrder(node);
				// 다시 순서 계산
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

				// 자신 뿐아니라 자식들까지 초기화
				ResetOrder(node);

				map<int, Node*>::iterator iter = nodes.begin();
				for (; iter != nodes.end(); iter++) {
					if (iter->second == node) {
						nodes.erase(iter);
						break;
					}
				}

				// 다시 순서 계산
				Order();
			}

			if (ImGui::TreeNode("Decorators")) {

				for (Blackboard board : blackboards) {
					string name = board.Name + "##add_decorators";
					if (board.Tasks.size() != 0) {
						if (ImGui::TreeNode(name.c_str())) {

							for (string decorator : board.Decorators) {
								name = decorator + "##add_decorator";
								// Root면 Decorator 추가 못함
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
								// Root면 Service 추가 못함
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
		
		orderNum = 0; // 시작 번호

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
	// Root로 들어오는 건 없음
	if (end->Type != NodeType::Root) {
		// 부모가 없는 경우 바로 이어버림
		if (end->Parent == NULL) {
			start->Childs.push_back(end);
			end->Parent = start;
		}
		// 있으면 끊고 이어버리기
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

		// 순서 정렬
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

		// 항상 root부터 시작
		Progress(nodes[0]);

		//// runningNode가 있지 않고(NULL이면) stk이 비어 잇으면 Root Node 부터 시작
		//if (runningNode == NULL) 
		//	Progress(nodes[0]);
		//else {
		//	bool result = Progress(runningNode);

		//	// running 이 끝난 경우
		//	if (result == true) {
		//		Node* temp = stk.top();
		//		// stack에서 꺼낸 Node가 root인 경우 runningNode의 null 넣기
		//		if(temp->Type == NodeType::Root)
		//			runningNode = NULL;
		//		else if (temp->Type == NodeType::Composite_Selector) {
		//			// true인 경우 selector true로 끝
		//			if (result == true) {

		//			}
		//			// false인 경우 다음 자식 or selector false로 끝
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
				// decorator의 반환값이 false면 false return하여 다시 root부터 시작
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
			// 루트는 그냥 자식 실행해서 결과값만 받기 false인지만 체크 
			// 결과값이 false면 다음 Update에서는 root부터 다시 시작함
			for (Node* child : node->Childs) {
				result &= Progress(child);

				//stk.pop();
			}
			break;
		}
		case Composite_Selector:
		{
			bool check = false;
			// 자식 중 하나가 실행에 성공하면 자손의 실행 멈춤 
			// 하나라도 실행 성공하면 Selector 성공
			// 모든 자손 실행 실패하면 실패
			for (Node* child : node->Childs) {
				bool temp = Progress(child);

				//// 자식이 끝나고 running 중이 있는 경우 break
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
			// 자식 중 하나가 실행에 실패하면 실행 멈춤
			// 모든 자손 실행이 성공해야 Sequence 성공
			// 자손 하나라도 실패하면 실패
			for (Node* child : node->Childs) {
				bool temp = Progress(child);

				//// 자식이 끝나고 running 중이 있는 경우 break
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
			// 블랙보드에서 task 찾아서 실행
			for (Blackboard& board : blackboards) {
				if (board.TaskFuncs.find(task) != board.TaskFuncs.end()) {
					TaskResult taskResult = board.TaskFuncs[task]();
					// task가 실행이 전부 끝나지 않으면 다음 Update때 해당 task부터 다시 시작 (runningNode로 기억)
					if (taskResult == TaskResult::Result_Running) {
						//runningNode = node;

						// 부모의 composite 타입에 따라 running이 지속되게 (다음 자식으로 가지 않게) 구현
						if (node->Parent->Type == NodeType::Composite_Selector)
							result = true;
						else if (node->Parent->Type == NodeType::Composite_Sequence)
							result = false;
					}
					// running이 끝나면 runningNode null로 구분
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
