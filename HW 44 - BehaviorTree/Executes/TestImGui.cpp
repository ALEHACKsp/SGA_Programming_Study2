#include "stdafx.h"
#include "TestImGui.h"

#include "Utilities\Gizmo.h"
#include "Draw\Nodes.h"
#include "Draw\BehaviorTree.h"

#include "Draw/MeshCube.h"

#include "Viewer/RenderTargetView.h"
#include "Viewer/DepthStencilView.h"
#include "Viewer/Viewport.h"

#include "Objects\TestUnit.h"

TestImGui::TestImGui()
{
	unit = new TestUnit();

	behaviorTree = new BehaviorTree();
	behaviorTree->AddBlackboard(unit);
	behaviorTree->Init();

	meshMaterial = new Material(Shaders + L"055_Mesh.fx");
	meshMaterial->SetAmbient(1.0f, 1.0f, 1.0f);
	meshMaterial->SetDiffuse(1.0f, 1.0f, 1.0f);
	meshMaterial->SetSpecular(0.8f, 0.8f, 0.8f, 16.0f);
	meshMaterial->SetDiffuseMap(L"White.png");

	cube = new MeshCube(meshMaterial, 1, 1, 1);

	rtv = new RenderTargetView(1280, 720);
	dsv = new DepthStencilView(1280, 720);

	D3DXMatrixIdentity(&matCube);
}


TestImGui::~TestImGui()
{	
	SAFE_DELETE(unit);
	SAFE_DELETE(behaviorTree);

	SAFE_DELETE(cube);
	SAFE_DELETE(meshMaterial);

	SAFE_DELETE(rtv);
	SAFE_DELETE(dsv);
}

void TestImGui::Update()
{
	behaviorTree->Update();
}

void TestImGui::PreRender()
{
	Context::Get()->GetViewport()->RSSetViewport();

	D3D::Get()->SetRenderTarget(rtv->RTV(), dsv->DSV());
	D3D::Get()->Clear(4283782502U, rtv->RTV(), dsv->DSV());

	cube->Render();
}

void TestImGui::Render()
{
	unit->Render();

	static bool open = false;
	ImGui::Checkbox("Node Graph Demo", &open);
	if (open)
		ImGuiNodeGraph(&open);

	static ImGui::Nodes nodes_;

	static bool open2 = false;
	ImGui::Checkbox("Node Graph Demo2", &open2);
	if (open2) {
		ImGui::Begin("Nodes");
		nodes_.ProcessNodes();
		ImGui::End();
	}

	static bool open3 = true;
	ImGui::Checkbox("Behavior Tree", &open3);
	if (open3) {
		behaviorTree->Render();
	}

#pragma region ImGui Render & Gizmo Test
	ImGui::Begin("Cube");
	D3DXVECTOR3 pos;
	cube->Position(&pos);
	if (ImGui::DragFloat3("Pos##Cube", (float*)&pos, 0.1f)) {
		cube->Position(pos);
	}
	D3DXVECTOR3 scale;
	cube->Scale(&scale);
	if (ImGui::DragFloat3("Scale##Cube", (float*)&scale, 0.1f)) {
		cube->Scale(scale);
	}
	D3DXVECTOR3 rot;
	cube->RotationDegree(&rot);
	if (ImGui::DragFloat3("Rot##Cube", (float*)&rot, 0.1f)) {
		cube->RotationDegree(rot);
	}

	ImGui::End();

	ImGui::Begin("Gizmo");

	static ImGuizmo::OPERATION operation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mode(ImGuizmo::LOCAL);

	ImGui::Columns(4);
	ImGui::RadioButton("T", (int*)&operation, ImGuizmo::OPERATION::TRANSLATE);
	ImGui::NextColumn();
	ImGui::RadioButton("R", (int*)&operation, ImGuizmo::OPERATION::ROTATE);
	ImGui::NextColumn();
	ImGui::RadioButton("S", (int*)&operation, ImGuizmo::OPERATION::SCALE);
	ImGui::NextColumn();
	ImGui::RadioButton("B", (int*)&operation, ImGuizmo::OPERATION::BOUNDS);
	ImGui::Columns(2);
	ImGui::RadioButton("L", (int*)&mode, ImGuizmo::LOCAL);
	ImGui::NextColumn();
	ImGui::RadioButton("W", (int*)&mode, ImGuizmo::WORLD);
	ImGui::Columns();
	ImGui::End();

	ImGui::Begin("Window", 0, ImGuiWindowFlags_NoMove);

	ImVec2 framePos = ImGui::GetCursorPos() + ImGui::GetWindowPos();
	ImVec2 frameSize = ImGui::GetWindowContentRegionMax() - ImGui::GetWindowContentRegionMin();

	frameSize.x -= static_cast<float>((static_cast<int>(frameSize.x) % 2 != 0) ? 1 : 0);
	frameSize.y -= static_cast<float>((static_cast<int>(frameSize.y) % 2 != 0) ? 1 : 0);

	ImGui::Image(rtv->SRV(), frameSize);

	D3DXMATRIX V, P;
	V = Context::Get()->GetView();
	P = Context::Get()->GetProjection();

	D3DXMATRIX W;
	D3DXMatrixIdentity(&W);

	//ImGuizmo::DrawGrid((float*)V, (float*)P, (float*)W, 10.0f);

	cube->Matrix(&W);

	ImGuizmo::SetDrawlist();
	ImGuizmo::SetRect(framePos.x, framePos.y, frameSize.x, frameSize.y);

	//ImGuizmo::DrawCube((float*)V, (float*)P, (float*)matCube);

	// 난 할 필요 없음
	//D3DXMatrixTranspose(&V, &V);
	//D3DXMatrixTranspose(&P, &P);
	//D3DXMatrixTranspose(&W, &W);

	//EditTransform((float*)V, (float*)P, (float*)matCube);

	ImGuizmo::Manipulate((float*)V, (float*)P, operation, mode, (float*)W);

	cube->SetMatrix(W);

	ImGui::End();
#pragma endregion
}

void TestImGui::PostRender()
{

}

// 구글링해서 찾은 ImGui Node Graph
void TestImGui::ImGuiNodeGraph(bool * opened)
{
	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiSetCond_FirstUseEver);
	if (!ImGui::Begin("Example: Custom Node Graph", opened))
	{
		ImGui::End();
		return;
	}

	// Dummy
	struct Node
	{
		int     ID;
		char    Name[32];
		ImVec2  Pos, Size;
		float   Value;
		ImVec4  Color;
		int     InputsCount, OutputsCount;

		Node(int id, const char* name, const ImVec2& pos, float value, const ImVec4& color, int inputs_count, int outputs_count) { ID = id; strncpy(Name, name, 31); Name[31] = 0; Pos = pos; Value = value; Color = color; InputsCount = inputs_count; OutputsCount = outputs_count; }

		ImVec2 GetInputSlotPos(int slot_no) const { return ImVec2(Pos.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)InputsCount + 1)); }
		ImVec2 GetOutputSlotPos(int slot_no) const { return ImVec2(Pos.x + Size.x, Pos.y + Size.y * ((float)slot_no + 1) / ((float)OutputsCount + 1)); }
	};
	struct NodeLink
	{
		int     InputIdx, InputSlot, OutputIdx, OutputSlot;

		NodeLink(int input_idx, int input_slot, int output_idx, int output_slot) { InputIdx = input_idx; InputSlot = input_slot; OutputIdx = output_idx; OutputSlot = output_slot; }
	};

	static ImVector<Node> nodes;
	static ImVector<NodeLink> links;
	static bool inited = false;
	static ImVec2 scrolling = ImVec2(0.0f, 0.0f);
	static bool show_grid = true;
	static int node_selected = -1;
	if (!inited)
	{
		nodes.push_back(Node(0, "MainTex", ImVec2(40, 50), 0.5f, ImColor(255, 100, 100), 1, 1));
		nodes.push_back(Node(1, "BumpMap", ImVec2(40, 150), 0.42f, ImColor(200, 100, 200), 1, 1));
		nodes.push_back(Node(2, "Combine", ImVec2(270, 80), 1.0f, ImColor(0, 200, 100), 2, 2));
		links.push_back(NodeLink(0, 0, 2, 0));
		links.push_back(NodeLink(1, 0, 2, 1));
		inited = true;
	}

	// Draw a list of nodes on the left side
	bool open_context_menu = false;
	int node_hovered_in_list = -1;
	int node_hovered_in_scene = -1;
	ImGui::BeginChild("node_list", ImVec2(100, 0));
	ImGui::Text("Nodes");
	ImGui::Separator();
	for (int node_idx = 0; node_idx < nodes.Size; node_idx++)
	{
		Node* node = &nodes[node_idx];
		ImGui::PushID(node->ID);
		if (ImGui::Selectable(node->Name, node->ID == node_selected))
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
	ImGui::SameLine(ImGui::GetWindowWidth() - 100);
	ImGui::Checkbox("Show grid", &show_grid);
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

	// Display links
	draw_list->ChannelsSplit(2);
	draw_list->ChannelsSetCurrent(0); // Background
	for (int link_idx = 0; link_idx < links.Size; link_idx++)
	{
		NodeLink* link = &links[link_idx];
		Node* node_inp = &nodes[link->InputIdx];
		Node* node_out = &nodes[link->OutputIdx];
		ImVec2 p1 = offset + node_inp->GetOutputSlotPos(link->InputSlot);
		ImVec2 p2 = offset + node_out->GetInputSlotPos(link->OutputSlot);
		draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f);
	}

	// Display nodes
	for (int node_idx = 0; node_idx < nodes.Size; node_idx++)
	{
		Node* node = &nodes[node_idx];
		ImGui::PushID(node->ID);
		ImVec2 node_rect_min = offset + node->Pos;

		// Display node contents first
		draw_list->ChannelsSetCurrent(1); // Foreground
		bool old_any_active = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);
		ImGui::BeginGroup(); // Lock horizontal position
		ImGui::Text("%s", node->Name);
		ImGui::SliderFloat("##value", &node->Value, 0.0f, 1.0f, "Alpha %.2f");
		ImGui::ColorEdit3("##color", &node->Color.x);
		ImGui::EndGroup();

		// Save the size of what we have emitted and whether any of the widgets are being used
		bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
		node->Size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
		ImVec2 node_rect_max = node_rect_min + node->Size;

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

		ImU32 node_bg_color = (node_hovered_in_list == node->ID || node_hovered_in_scene == node->ID || (node_hovered_in_list == -1 && node_selected == node->ID)) ? IM_COL32(75, 75, 75, 255) : IM_COL32(60, 60, 60, 255);
		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);
		for (int slot_idx = 0; slot_idx < node->InputsCount; slot_idx++)
			draw_list->AddCircleFilled(offset + node->GetInputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
		for (int slot_idx = 0; slot_idx < node->OutputsCount; slot_idx++)
			draw_list->AddCircleFilled(offset + node->GetOutputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));

		ImGui::PopID();
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
		Node* node = node_selected != -1 ? &nodes[node_selected] : NULL;
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
		if (node)
		{
			ImGui::Text("Node '%s'", node->Name);
			ImGui::Separator();
			if (ImGui::MenuItem("Rename..", NULL, false, false)) {}
			if (ImGui::MenuItem("Delete", NULL, false, false)) {}
			if (ImGui::MenuItem("Copy", NULL, false, false)) {}
		}
		else
		{
			if (ImGui::MenuItem("Add")) { nodes.push_back(Node(nodes.Size, "New node", scene_pos, 0.5f, ImColor(100, 100, 200), 2, 2)); }
			if (ImGui::MenuItem("Paste", NULL, false, false)) {}
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
