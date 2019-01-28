#include "stdafx.h"
#include "BehaviorTree.h"


BehaviorTree::BehaviorTree(bool inited)
	: show_grid(true), node_selected(-1), scrolling(ImVec2(0,0))
	, link_node(false)
{
	if (inited)
	{
		nodes.insert(make_pair(NodeId++, new Node(NodeType::Root, ImVec2(40, 50))));
		nodes.insert(make_pair(NodeId++, new Node(NodeType::Composite_Selector, ImVec2(40, 150))));
		nodes.insert(make_pair(NodeId++, new Node(NodeType::Composite_Sequence, ImVec2(270, 80))));
		nodes.insert(make_pair(NodeId++, new Node(NodeType::Task, ImVec2(140, 130))));
		nodes.insert(make_pair(NodeId++, new Node(NodeType::Task, ImVec2(270, 130), "Move")));
	}
}

BehaviorTree::~BehaviorTree()
{
	for (pair<const int, Node*>& p : nodes)
		SAFE_DELETE(p.second);
}

void BehaviorTree::Render()
{
	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Behavior Tree"))
	{
		ImGui::End();
		return;
	}

	// Draw a list of nodes on the left side
	bool open_context_menu = false;
	int node_hovered_in_scene = -1;
	int node_hovered_in_list = -1;

	ImGui::BeginChild("node_list", ImVec2(100, 0));
	ImGui::Text("Nodes");
	ImGui::Separator();
	for (pair<const int, Node*>& p : nodes)
	{
		Node* node = p.second;
		ImGui::PushID(node->ID);
		if (ImGui::Selectable(node->Label().c_str(), node->ID == node_selected))
			node_selected = node->ID;
		if (ImGui::IsItemHovered())
		{
			node_hovered_in_list = node->ID;
			open_context_menu |= ImGui::IsMouseClicked(1);
		}
		ImGui::PopID();
	}
	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginGroup();

	const float NODE_SLOT_RADIUS = 4.0f;
	const ImVec2 NODE_WINDOW_PADDING(8.0f, 8.0f);

	// Create our child canvas
	ImGui::Text("Hold middle mouse button to scroll (%.2f,%.2f)", scrolling.x, scrolling.y);

	
	for (int i = 0; i < NodeType::End; i++) {
		ImGui::SameLine(ImGui::GetWindowWidth() - 335 + i * 25);
		string label = "##Color" + to_string(i);
		ImGui::ColorEdit4(label.c_str(), (float*)&NodeColor[i], ImGuiColorEditFlags_NoInputs);
	}

	ImGui::SameLine(ImGui::GetWindowWidth() - 180);
	ImGui::Checkbox("Grid", &show_grid);
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

		ImGui::Text("%s", node->Name.c_str());

		ImGui::EndGroup();

		// Save the size of what we have emitted and whether any of the widgets are being used
		bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
		node->Size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
		ImVec2 node_rect_max = node_rect_min + node->Size;

		// Order
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

		ImColor node_bg_color = (node_hovered_in_list == node->ID || node_hovered_in_scene == node->ID || (node_hovered_in_list == -1 && node_selected == node->ID)) ? 
			node->Color + ImVec4(0.1f,0.1,0.1f, 0.0f) : node->Color;
		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);
		// Root는 들어오는 값 없음
		if (node->Type != NodeType::Root)
			draw_list->AddCircleFilled(offset + node->InDegreePos(), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
		// Task에서 나가는 값은 없음
		if (node->Type != NodeType::Task)
			draw_list->AddCircleFilled(offset + node->OutDegreePos(), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));

		if (node->Order != -1)
			draw_list->AddCircleFilled(orderPos + ImVec2(8.25f,7), 10, IM_COL32(170, 170, 170, 150));
		
		ImGui::PopID();
	}

	// link node
	if (link_node && ImGui::IsMouseReleased(0)) {
		link_node = false;
		if (node_hovered_in_scene != -1 && start_node != node_hovered_in_scene) {
			end_node = node_hovered_in_scene;
			Node* start = nodes[start_node];
			Node* end = nodes[end_node];
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
		node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
		open_context_menu = true;
	}
	if (open_context_menu)
	{
		ImGui::OpenPopup("context_menu");
		if (node_hovered_in_list != -1)
			node_selected = node_hovered_in_list;
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
			}
			if (ImGui::MenuItem("Delete Node", NULL)) {
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

				map<int, Node*>::iterator iter = nodes.begin();
				for (; iter != nodes.end(); iter++) {
					if (iter->second == node) {
						nodes.erase(iter);
						break;
					}
				}
			}
		}
		else
		{
			if (ImGui::MenuItem("Add", NULL, false, false)) {}
		}
		ImGui::EndPopup();
	}
	ImGui::PopStyleVar();

	// Scrolling
	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
		scrolling = scrolling + ImGui::GetIO().MouseDelta;

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
		
		DFS(nodes[0], 0);
	}
}

void BehaviorTree::DFS(Node * node, int order)
{
	node->Order = order;
	check[node->ID] = true;

	int nextOrder = order + 1;

	for (Node * child : node->Childs) {
		if (check[child->ID] == false) {
			DFS(child, nextOrder);
		}
	}

}
