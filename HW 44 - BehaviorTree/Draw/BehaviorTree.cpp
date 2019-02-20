#include "stdafx.h"
#include "BehaviorTree.h"

#include "Draw/Unit.h"

BehaviorTree::BehaviorTree()
	: show_grid(true), node_selected(-1), scrolling(ImVec2(0,0))
	, link_node(false), bProgress(false), bRunning(false)
	, deltaTime(0), rate(3.0f), saveFolder(L"./BehaviorTree")
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

	if (ImGui::Checkbox("Progress", &bProgress))
		ResetStatus();

	ImGui::DragFloat("Rate", &rate, 0.1f, 0.1f, 5.0f);

	if (ImGui::Button("Reset")) Reset();
	ImGui::Columns(2, 0, false);
	if (ImGui::Button("Save")) {
		Path::CreateFolders(saveFolder);
		function<void(wstring)> func = bind(&BehaviorTree::Save, this, placeholders::_1);
		Path::SaveFileDialog(L"", L"BT Json Files(*.json)\0*.json\0", saveFolder, func);
	}
	ImGui::NextColumn();
	if (ImGui::Button("Load")) {
		Path::CreateFolders(saveFolder);
		function<void(wstring)> func = bind(&BehaviorTree::Load, this, placeholders::_1);
		Path::OpenFileDialog(L"", L"BT Json Files(*.json)\0*.json\0", saveFolder, func);
	}
	ImGui::Columns();

	ImGui::Separator();

	//ImGui::Text("Keys");
	//ImGui::Separator();

	//for (Blackboard& board : blackboards) {
	//	string name = board.Name + "##key_list";
	//	if (board.Keys.size() != 0) {
	//		if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
	//			for (string& key : board.Keys) {
	//				ImGui::BulletText(key.c_str());
	//				if (board.KeyTips.find(key) != board.KeyTips.end()) {
	//					if (ImGui::IsItemHovered()) {
	//						string text = key + " : " + board.KeyTips[key];
	//						ImGui::BeginTooltip();
	//						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
	//						ImGui::Text(text.c_str());
	//						ImGui::PopTextWrapPos();
	//						ImGui::EndTooltip();
	//					}
	//				}
	//			}
	//			ImGui::TreePop();
	//		}
	//	}
	//}

	ImGui::Text("Tasks");
	ImGui::Separator();

	for (Blackboard& board : blackboards) {
		string name = board.Name + "##task_list";
		if (board.Tasks.size() != 0) {
			if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
				for (string& task : board.Tasks) {
					ImGui::BulletText(task.c_str());
					// tooltip 있으면 마우스 hovering시 보여주기
					if (board.TaskTips.find(task) != board.TaskTips.end()) {
						if (ImGui::IsItemHovered()) {
							string text = task + " : " + board.TaskTips[task];
							ImGui::BeginTooltip();
							ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
							ImGui::Text(text.c_str());
							ImGui::PopTextWrapPos();
							ImGui::EndTooltip();
						}
					}
				}
				ImGui::TreePop();
			}
		}
	}

	ImGui::Text("Services");
	ImGui::Separator();

	for (Blackboard& board : blackboards) {
		string name = board.Name + "##service_list";
		if (board.Services.size() != 0) {
			if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
				for (string& service : board.Services) {
					ImGui::BulletText(service.c_str());
					// tooltip 있으면 마우스 hovering시 보여주기
					if (board.ServicesTips.find(service) != board.ServicesTips.end()) {
						if (ImGui::IsItemHovered()) {
							string text = service + " : " + board.ServicesTips[service];
							ImGui::BeginTooltip();
							ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
							ImGui::Text(text.c_str());
							ImGui::PopTextWrapPos();
							ImGui::EndTooltip();
						}
					}
				}
				ImGui::TreePop();
			}
		}
	}

	ImGui::Text("Decorators");
	ImGui::Separator();

	for (Blackboard& board : blackboards) {
		string name = board.Name + "##decorator_list";
		if (board.Decorators.size() != 0) {
			if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen)) {
				for (string& decorator : board.Decorators) {
					ImGui::BulletText(decorator.c_str());
					// tooltip 있으면 마우스 hovering시 보여주기
					if (board.DecoratorsTips.find(decorator) != board.DecoratorsTips.end()) {
						if (ImGui::IsItemHovered()) {
							string text = decorator + " : " + board.DecoratorsTips[decorator];
							ImGui::BeginTooltip();
							ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
							ImGui::Text(text.c_str());
							ImGui::PopTextWrapPos();
							ImGui::EndTooltip();
						}
					}
				}
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
	for (int i = 0; i < NodeType::End + 2; i++) {
		string label = "##Color" + to_string(i);
		ImGui::SameLine((float)(i + 1) * 25 + 50);
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

		ImColor node_color = node->Running == true ? NodeColor[7] : IM_COL32(100, 100, 100, 255);

		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		draw_list->AddRect(node_rect_min, node_rect_max, node_color, 4.0f);

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
			draw_list->AddBezierCurve(p1, p1 + ImVec2(0, +50), p2 + ImVec2(0, -50), p2, NodeColor[6], 3.0f);
		}

		for (Node* child : node->Childs) {
			ImVec2 p2 = offset + child->InDegreePos();
			ImColor color = child->Running == true ? NodeColor[7] : NodeColor[6];
			draw_list->AddBezierCurve(p1, p1 + ImVec2(0, +50), p2 + ImVec2(0, -50), p2, color, 3.0f);
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

			if (ImGui::TreeNodeEx("Add", ImGuiTreeNodeFlags_DefaultOpen)) {

				if (ImGui::MenuItem("Link", "Add Link", false, node->Type != NodeType::Task)) {
					link_node = true;
					start_node = node->ID;
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
									// tooltip 있으면 마우스 hovering시 보여주기
									if (board.DecoratorsTips.find(decorator) != board.DecoratorsTips.end()) {
										if (ImGui::IsItemHovered()) {
											string text = decorator + " : " + board.DecoratorsTips[decorator];
											ImGui::BeginTooltip();
											ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
											ImGui::Text(text.c_str());
											ImGui::PopTextWrapPos();
											ImGui::EndTooltip();
										}
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
									// tooltip 있으면 마우스 hovering시 보여주기
									if (board.ServicesTips.find(service) != board.ServicesTips.end()) {
										if (ImGui::IsItemHovered()) {
											string text = service + " : " + board.ServicesTips[service];
											ImGui::BeginTooltip();
											ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
											ImGui::Text(text.c_str());
											ImGui::PopTextWrapPos();
											ImGui::EndTooltip();
										}
									}
								}

								ImGui::TreePop();
							}
						}
					}

					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx("Delete", ImGuiTreeNodeFlags_DefaultOpen)) {

				if (ImGui::MenuItem("Link", "Delete InDegree Link")) {
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

				if (ImGui::MenuItem("Node", "Delete Node", false, node->Type != NodeType::Root)) {
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

				if(ImGui::TreeNodeEx("Decorators", ImGuiTreeNodeFlags_DefaultOpen)) {
					vector<string>::iterator iter = node->Decorators.begin();
					for (; iter != node->Decorators.end();) {
						string name = *iter + "##delete_decorator";

						if (ImGui::MenuItem(name.c_str()))
							iter = node->Decorators.erase(iter);
						else
							iter++;
					}

					ImGui::TreePop();
				}

				if (ImGui::TreeNodeEx("Services", ImGuiTreeNodeFlags_DefaultOpen)) {
					vector<string>::iterator iter = node->Services.begin();
					for (; iter != node->Services.end(); iter++) {
						string name = *iter + "##delete_service";

						if (ImGui::MenuItem(name.c_str()))
							iter = node->Services.erase(iter);
					}
					
					ImGui::TreePop();
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
								// tooltip 있으면 마우스 hovering시 보여주기
								if (board.TaskTips.find(task) != board.TaskTips.end()) {
									if (ImGui::IsItemHovered()) {
										string text = task + " : " + board.TaskTips[task];
										ImGui::BeginTooltip();
										ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
										ImGui::Text(text.c_str());
										ImGui::PopTextWrapPos();
										ImGui::EndTooltip();
									}
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

		for (pair<const int, bool> c : check)
			if (c.second == false)
				nodes[c.first]->Order = -1;
	}
}

void BehaviorTree::Reset(bool bRoot)
{
	for (pair<const int, Node*>& p : nodes)
		SAFE_DELETE(p.second);

	nodes.clear();
	NodeId = 0;

	if (bRoot == true)
		// root node
		nodes.insert(make_pair(NodeId++, new Node(NodeType::Root, ImVec2(64 * 3 + 16, 64 * 1 + 16))));
}

void BehaviorTree::ResetOrder(Node * node)
{
	node->Order = -1;

	for (Node* child : node->Childs)
		ResetOrder(child);
}

void BehaviorTree::ResetStatus(Node * node)
{
	// 처음 호출
	if (node == NULL) {
		bRunning = false;

		// Blackboard의 Reset Service가 있다면 찾아서 실행
		string service = "Reset"; // 해당 Service 구현해서 추가했어야함
		for (Blackboard& board : blackboards) {
			if (board.ServiceFuncs.find(service) != board.ServiceFuncs.end()) {
				board.ServiceFuncs[service]();
				break;
			}
		}

		ResetStatus(nodes[0]);
	}
	else {
		node->Running = false;

		switch (node->Type)
		{
		case Root:
		case Composite_Selector:
		case Task:
			node->Status = NodeStatus::Status_False;
			break;
		case Composite_Sequence:
			node->Status = NodeStatus::Status_True;
			break;
		}


		for (Node* child : node->Childs)
			ResetStatus(child);
	}
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
			// 시작이 Root고 root가 이미 이어져있다면 끊어버려야함
			if (start->Type == NodeType::Root && start->Childs.size() > 0) {
				for (Node* child : start->Childs)
					child->Parent = NULL;
				start->Childs.clear();
			}

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

		sort(start->Childs.begin(), start->Childs.end());

		// 순서 정렬
		Order();
	}
}

void BehaviorTree::Save(wstring fileName)
{
	string jsonName = "BehaviorTree";

	json.Root().clear();

	for (pair<const int, Node*> p : nodes) {
		Node* node = p.second;

		Json::Value record;

		record["ID"] = node->ID;
		record["Name"] = node->Name;
		record["Position"][0] = node->Pos.x;
		record["Position"][1] = node->Pos.y;

		string type;
		switch (node->Type)
		{
		case Root: type = "Root"; break;
		case Composite_Selector: type = "Selector"; break;
		case Composite_Sequence: type = "Sequence"; break;
		case Task: type = "Task"; break;
		}
		record["NodeType"] = type;

		if (node->Parent != NULL)
			record["Parent"] = node->Parent->ID;
		else
			record["Parent"] = -1;
		
		for (UINT i = 0; i < node->Childs.size(); i++)
			record["Childs"][i] = node->Childs[i]->ID;
			
		for (UINT i = 0; i < node->Decorators.size(); i++)
			record["Decorators"][i] = node->Decorators[i];
		for (UINT i = 0; i < node->Services.size(); i++)
			record["Services"][i] = node->Services[i];

		json.Root()[jsonName].append(record);
	}

	string str = String::ToString(fileName);
	json.Save(str);
}

void BehaviorTree::Load(wstring fileName)
{
	string jsonName = "BehaviorTree";

	json.Root().clear();

	string str = String::ToString(fileName);
	json.Read(str);

	Reset(false);

	for (Json::Value& v : json.SubTree(jsonName))
	{
		string typeStr = v["NodeType"].asString();
		NodeType type;
		if (typeStr == "Root") type = NodeType::Root;
		if (typeStr == "Selector") type = NodeType::Composite_Selector;
		if (typeStr == "Sequence") type = NodeType::Composite_Sequence;
		if (typeStr == "Task") type = NodeType::Task;

		ImVec2 pos;
		pos.x = v["Position"][0].asFloat();
		pos.y = v["Position"][1].asFloat();

		string name = v["Name"].asString();

		Node* node = new Node(type, pos, name);

		for (UINT i = 0; i < v["Decorators"].size(); i++)
			node->Decorators.push_back(v["Decorators"][i].asString());

		for (UINT i = 0; i < v["Services"].size(); i++)
			node->Decorators.push_back(v["Services"][i].asString());

		int id = v["ID"].asInt();
		node->ID = id;

		nodes.insert(make_pair(node->ID, node));
		NodeId = max(node->ID, NodeId);
	}
	NodeId++; // 등록된 id 다음꺼부터 새로 등록하기 위해 ++
	
	// Link 연결

	for (Json::Value& v : json.SubTree(jsonName))
	{
		int id = v["ID"].asInt();

		int parentId = v["Parent"].asInt();

		if (parentId != -1) {
			Node* parent = Find(parentId);
			nodes[id]->Parent = parent;
		}
		else
			nodes[id]->Parent = NULL;

		for (UINT i = 0; i < v["Childs"].size(); i++) {
			int childId = v["Childs"][i].asInt();
			Node* child = Find(childId);
			if (child != NULL) {
				nodes[id]->Childs.push_back(child);
			}
		}
	}

	Order();
}

Node * BehaviorTree::Find(int id)
{
	if (nodes.find(id) != nodes.end()) 
		return nodes.find(id)->second;
	else
		return NULL;
}

void BehaviorTree::AddBlackboard(IBlackboard * blackboard)
{
	Blackboard board;

	board.Name = blackboard->BlackboardName();
	//blackboard->BlackboardKeys(board.Keys, board.KeyFuncs, board.KeyTips);
	blackboard->BlackboardTasks(board.Tasks, board.TaskFuncs, board.TaskTips);
	blackboard->BlackboardServices(board.Services, board.ServiceFuncs, board.ServicesTips);
	blackboard->BlackboardDecorators(board.Decorators, board.DecoratorFuncs, board.DecoratorsTips);

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
			nodes.insert(make_pair(NodeId++, new Node(NodeType::Task, ImVec2((float)(64 + 64 + 32 + 16) * i++ + 16, (float)64 * 3 + 16), task)));
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

		// 매 업데이트때 status 초기 상태로 두고 시작 running 상태시 reset 안함
		if (bRunning == false)
			ResetStatus(nodes[0]);

		// 항상 root부터 시작
		Progress(nodes[0]);
	}
}

bool BehaviorTree::Progress(Node * node)
{
	// running 상태일땐 decorartor service 작동 안하게
	if (bRunning == false) {
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
	}
	// Composite or Task

	bool check = node->Status == Status_False ? false : true;

	switch (node->Type)
	{
		case Root: 
		{
			node->Running = true;

			// 루트는 그냥 자식 실행하기만 하면 됨 (따로 처리할 필요 없음)
			// 루트는 자식 하나여야함
			for (Node* child : node->Childs) {
				Progress(child);
			}
			break;
		}
		case Composite_Selector:
		{
			node->Running = true;

			// 자식 중 하나가 실행에 성공하면 자손의 실행 멈춤 
			// 하나라도 실행 성공하면 Selector 성공
			// 모든 자손 실행 실패하면 실패
			for (Node* child : node->Childs) {
				bool temp = Progress(child);

				// 자식의 상태가 running인 경우 break
				if (child->Status == Status_Running)
					break;

				check |= temp;

				if (check == true)
					break;
			}
			// running 상태가 아니면 상태 저장 (running 상태인 경우 이전 상태로 놔둠)
			if (bRunning == false)
				node->Status = (NodeStatus)check;
			break;
		}
		case Composite_Sequence:
		{
			node->Running = true;

			// 자식 중 하나가 실행에 실패하면 실행 멈춤
			// 모든 자손 실행이 성공해야 Sequence 성공
			// 자손 하나라도 실패하면 실패
			for (Node* child : node->Childs) {
				bool temp = Progress(child);

				// 자식의 상태가 running인 경우 break
				if (child->Status == Status_Running)
					break;

				check &= temp;

				if (check == false)
					break;
			}
			// running 상태가 아니면 상태 저장 (running 상태인 경우 이전 상태로 진행)
			if (bRunning == false)
				node->Status = (NodeStatus)check;
			break;
		}
		case Task:
		{
			// running 중이 아니거나 running 일 땐 running 상태인 node만 task 실행
			if (bRunning == false || (bRunning == true && node->Status == NodeStatus::Status_Running)) {
				node->Running = true;

				string task = node->Name;
				// 블랙보드에서 task 찾아서 실행
				for (Blackboard& board : blackboards) {
					if (board.TaskFuncs.find(task) != board.TaskFuncs.end()) {
						TaskResult taskResult = board.TaskFuncs[task]();
						
						// task가 running이면 running 상태 돌입
						if (taskResult == TaskResult::Result_Running) {
							node->Status = NodeStatus::Status_Running;
							bRunning = true;
						}
						// running이 끝나면 running을 끝내고 해당 결과 상태에 저장
						else {
							node->Status = (NodeStatus)taskResult;
							bRunning = false;
						}

						break;
					}
				}
			}
			else {
				node->Running = false;
			}

			break;
		}
	}

	return node->Status == Status_False ? false : true;
}
