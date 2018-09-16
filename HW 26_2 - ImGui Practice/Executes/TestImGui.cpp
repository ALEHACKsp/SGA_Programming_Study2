#include "stdafx.h"
#include "TestImGui.h"

#include "../Objects/GameAnimModel.h"

TestImGui::TestImGui(ExecuteValues * values)
	:Execute(values)
{

}

TestImGui::~TestImGui()
{

}

void TestImGui::Update()
{

}

void TestImGui::PreRender()
{

}

void TestImGui::Render()
{
	// 사용되는 변수들 귀찮아 static으로
	static int selectListNum = -1;
	static int selectItem = -1;
	static int style;
	static ImVec2 itemMin;
	static ImVec2 itemMax;
	static ImVec2 mousePos;
	static ImVec2 win1Pos;
	static ImVec2 win2Pos;
	static ImVec2 win3Pos;
	static ImVec2 win0Pos;
	static string itemName;
	static vector<Item> items;
	static bool itemSelect = false;
	
	mousePos = ImGui::GetMousePos();

	// Debug (Based)
	{
		ImGui::StyleColorsDark();
		win0Pos = ImGui::GetWindowPos();
		ImGui::Text("Debug Pos (%.2f %.2f)", win0Pos.x, win0Pos.y);
		ImGui::SameLine(200);
		ImGui::Text("DrawList Pos (%.2f %.2f)", win1Pos.x, win1Pos.y);
		ImGui::SameLine(400);
		ImGui::Text("Main Pos (%.2f %.2f)", win2Pos.x, win2Pos.y);
		ImGui::SameLine(600);
		ImGui::Text("DrawInfo Pos (%.2f %.2f)", win3Pos.x, win3Pos.y);
		ImGui::Text("Mouse Pos (%f %f)", mousePos.x, mousePos.y);
		switch (selectListNum)
		{
			case 0:  ImGui::Text("Item : Line");		break;
			case 1:  ImGui::Text("Item : Triangle");	break;
			case 2:  ImGui::Text("Item : Circle");		break;
			case 3:  ImGui::Text("Item : Quad");		break;
			default: ImGui::Text("Item : None");		break;
		}
		ImGui::SameLine(200);
		ImGui::Text("item's Rect Min %f %f, Max %f %f", itemMin.x, itemMin.y, itemMax.x, itemMax.y);
	}

	// Draw List
	{
		// 확인용
		ImGui::GetWindowDrawList();

		ImGui::StyleColorsClassic();
		ImGui::Begin("Draw List");
		win1Pos = ImGui::GetWindowPos();
		
		if (ImGui::IsWindowHovered())
			ImGui::SetWindowFocus();

		ImGui::RadioButton("Style1", &style, 0);
		ImGui::SameLine(120);
		ImGui::RadioButton("Style2", &style, 1);
		ImGui::Separator();

		// list
		{
			itemName = "Line";
			ImGui::InvisibleButton("Dummy", ImVec2(ImGui::GetWindowSize().x - 20, 15));
			itemMin = ImGui::GetItemRectMin();
			itemMax = ImGui::GetItemRectMax();

			if (selectListNum == 0) {
				if (style == 0)
					//ImGui::TextColored(ImVec4(1, 1, 0, 1), itemName.c_str());
					ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 0, 255), itemName.c_str());
				else if (style == 1) { 
					//ImGui::Text(itemName.c_str());
					ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 255, 0, 128));
					ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 255, 255), itemName.c_str());
				}
			}
			else
				//ImGui::Text(itemName.c_str());
				ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 255, 255), itemName.c_str());

			if (ImGui::IsItemClicked()) {
				selectListNum = 0;
				itemSelect = true;
			}

			itemName = "Triangle";
			ImGui::InvisibleButton("Dummy", ImVec2(ImGui::GetWindowSize().x - 20, 15));
			itemMin = ImGui::GetItemRectMin();
			itemMax = ImGui::GetItemRectMax();

			if (selectListNum == 1) {
				if (style == 0)
					ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 0, 255), itemName.c_str());
				else if (style == 1) {
					ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 255, 255), itemName.c_str());
					ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 255, 0, 128));
				}
			}
			else
				ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 255, 255), itemName.c_str());

			if (ImGui::IsItemClicked()) {
				selectListNum = 1;
				itemSelect = true;
			}

			itemName = "Rect";
			ImGui::InvisibleButton("Dummy", ImVec2(ImGui::GetWindowSize().x - 20, 15));
			itemMin = ImGui::GetItemRectMin();
			itemMax = ImGui::GetItemRectMax();

			if (selectListNum == 2) {
				if (style == 0)
					ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 0, 255), itemName.c_str());
				else if (style == 1) {
					ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 255, 255), itemName.c_str());
					ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 255, 0, 128));
				}
			}
			else
				ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 255, 255), itemName.c_str());

			if (ImGui::IsItemClicked()) {
				selectListNum = 2;
				itemSelect = true;
			}

			itemName = "Circle";
			ImGui::InvisibleButton("Dummy", ImVec2(ImGui::GetWindowSize().x - 20, 15));
			itemMin = ImGui::GetItemRectMin();
			itemMax = ImGui::GetItemRectMax();

			if (selectListNum == 3) {
				if (style == 0)
					ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 0, 255), itemName.c_str());
				else if (style == 1) {
					ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 255, 255), itemName.c_str());
					ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 255, 0, 128));
				}
			}
			else
				ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 255, 255), itemName.c_str());

			if (ImGui::IsItemClicked()) {
				selectListNum = 3;
				itemSelect = true;
			}

			itemName = "Quad";
			ImGui::InvisibleButton("Dummy", ImVec2(ImGui::GetWindowSize().x - 20, 15));
			itemMin = ImGui::GetItemRectMin();
			itemMax = ImGui::GetItemRectMax();

			if (selectListNum == 4) {
				if (style == 0)
					ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 0, 255), itemName.c_str());
				else if (style == 1) {
					ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 255, 255), itemName.c_str());
					ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 255, 0, 128));
				}
			}
			else
				ImGui::GetWindowDrawList()->AddText(itemMin, IM_COL32(255, 255, 255, 255), itemName.c_str());

			if (ImGui::IsItemClicked()) {
				selectListNum = 4;
				itemSelect = true;
			}
		}

		ImGui::End();
	}

	// Main 
	{
		ImGui::StyleColorsLight();
		ImGui::Begin("Main");
		win2Pos = ImGui::GetWindowPos();

		if (ImGui::IsMouseReleased(0)) {
			if (ImGui::IsWindowHovered() && itemSelect == true) {
				itemSelect = false;
				Item item;
				switch (selectListNum)
				{
					case 0:
						item.type = 0;
						item.name = "Line";
						item.a = mousePos;
						item.b = ImVec2(mousePos.x + 10, mousePos.y + 10);
						item.color = ImVec4(0, 0, 0, 1);
						items.push_back(item);
						break;
					case 1:
						item.type = 1;
						item.name = "Triangle";
						item.a = mousePos;
						item.b = ImVec2(mousePos.x + 10, mousePos.y);
						item.c = ImVec2(mousePos.x, mousePos.y + 10);
						item.color = ImVec4(0, 0, 0, 1);
						items.push_back(item);
						break;
					case 2:
						item.type = 2;
						item.name = "Rect";
						item.a = mousePos;
						item.b = ImVec2(mousePos.x + 10, mousePos.y + 10);
						item.color = ImVec4(0, 0, 0, 1);
						items.push_back(item);
						break;
					case 3:
						item.type = 3;
						item.name = "Circle";
						item.center = mousePos;
						item.radius = 5;
						item.color = ImVec4(0, 0, 0, 1);
						items.push_back(item);
						break;
					case 4:
						item.type = 4;
						item.name = "Quad";
						item.a = mousePos;
						item.b = ImVec2(mousePos.x, mousePos.y + 10);
						item.c = ImVec2(mousePos.x + 10, mousePos.y);
						item.d = ImVec2(mousePos.x + 10, mousePos.y + 10);
						item.color = ImVec4(0, 0, 0, 1);
						items.push_back(item);
						break;
					default:
						break;
				}
			}
			else
				itemSelect = false;
		}

		for (int i=0; i<items.size(); i++)
		{
			ImVec4 vec = items[i].color;
			ImU32 color = ImColor(vec.x, vec.y, vec.z, vec.w);
			switch (items[i].type)
			{
				case 0:
					ImGui::GetWindowDrawList()->AddLine(items[i].a, items[i].b, 
						color);
					break;
				case 1:
					if(i == selectItem)
						ImGui::GetWindowDrawList()->AddTriangle(items[i].a, items[i].b, items[i].c, 
							color);
					else
						ImGui::GetWindowDrawList()->AddTriangleFilled(items[i].a, items[i].b, items[i].c, 
							color);
					break;
				case 2:
					if (i == selectItem)
						ImGui::GetWindowDrawList()->AddRect(items[i].a, items[i].b, 
							color);
					else
						ImGui::GetWindowDrawList()->AddRectFilled(items[i].a, items[i].b, 
							color);
					break;
				case 3:
					if (i == selectItem)
						ImGui::GetWindowDrawList()->AddCircle(items[i].center, items[i].radius, 
							color);
					else
						ImGui::GetWindowDrawList()->AddCircleFilled(items[i].center, items[i].radius, 
							color);
					break;
				case 4:
					if(i == selectItem)
						ImGui::GetWindowDrawList()->AddQuad(items[i].a, items[i].b, items[i].c, items[i].d, 
							color);
					else
						ImGui::GetWindowDrawList()->AddQuadFilled(items[i].a, items[i].b, items[i].c, items[i].d, 
							color);
					break;
			}
		}

		ImGui::End();
	}
	
	// Draw Info
	{
		ImGui::StyleColorsClassic();
		ImGui::Begin("Draw Info");
		win3Pos = ImGui::GetWindowPos();

		for (int i = 0; i < items.size(); i++) {
			ImGui::Text("Name : %s", items[i].name.c_str());
			string id = items[i].name + " " + to_string(i);
			switch (items[i].type)
			{
				case 0:
					//ImGui::Text("Pos A : %.2f %.2f", items[i].a.x, items[i].a.y);
					//ImGui::Text("Pos B : %.2f %.2f", items[i].b.x, items[i].b.y);
					ImGui::DragFloat2((id + " a").c_str(), (float*)&items[i].a);
					ImGui::DragFloat2((id + " b").c_str(), (float*)&items[i].b);
					ImGui::ColorEdit4((id + " color").c_str(), (float*)&items[i].color);
					break;
				case 1:
					ImGui::DragFloat2((id + " a").c_str(), (float*)&items[i].a);
					ImGui::DragFloat2((id + " b").c_str(), (float*)&items[i].b);
					ImGui::DragFloat2((id + " c").c_str(), (float*)&items[i].c);
					ImGui::ColorEdit4((id + " color").c_str(), (float*)&items[i].color);
					break;
				case 2:
					ImGui::DragFloat2((id + " a").c_str(), (float*)&items[i].a);
					ImGui::DragFloat2((id + " b").c_str(), (float*)&items[i].b);
					ImGui::ColorEdit4((id + " color").c_str(), (float*)&items[i].color);
					break;
				case 3:
					ImGui::DragFloat2((id + " center").c_str(), (float*)&items[i].center);
					ImGui::DragFloat((id + " radius").c_str(), &items[i].radius);
					ImGui::ColorEdit4((id + " color").c_str(), (float*)&items[i].color);
					break;
				case 4:
					ImGui::DragFloat2((id + " a").c_str(), (float*)&items[i].a);
					ImGui::DragFloat2((id + " b").c_str(), (float*)&items[i].b);
					ImGui::DragFloat2((id + " c").c_str(), (float*)&items[i].c);
					ImGui::DragFloat2((id + " d").c_str(), (float*)&items[i].d);
					ImGui::ColorEdit4((id + " color").c_str(), (float*)&items[i].color);
					break;
				default:
					break;
			}

			ImGui::Separator();
		}

		ImGui::End();
	}
}

void TestImGui::PostRender()
{
	//static ImVec2 size(100,100), offset(50, 20);
	
	//// main menu bar
	{
		//if (ImGui::BeginMainMenuBar())
		//{
		//
		//	if (ImGui::BeginMenu("File"))
		//	{
		//		//ImGui::BeginTooltip();
		//		//ImGui::SetTooltip("test");
		//		//ImGui::EndTooltip();
		//		if (ImGui::MenuItem("Open")) {}
		//		if (ImGui::MenuItem("Save")) {}
		//
		//		ImGui::EndMenu();
		//	}
		//
		//	ImGui::EndMainMenuBar();
		//}
	}

	// Moues Drag Test
	{
		//ImGui::Begin("Mouse Drag", 0, ImGuiWindowFlags_MenuBar);
		//ImVec2 pos = ImGui::GetMousePos();
		//ImGui::Text("mouse pos %f %f", pos.x, pos.y);

		//if (ImGui::BeginMenuBar()) {
		//	if (ImGui::BeginMenu("File"))
		//	{
		//		//ImGui::BeginTooltip();
		//		//ImGui::SetTooltip("test");
		//		//ImGui::EndTooltip();
		//		if (ImGui::MenuItem("Open")) {}
		//		if (ImGui::MenuItem("Save")) {}

		//		ImGui::EndMenu();
		//	}
		//	ImGui::EndMenuBar();
		//}

		////ImVec2 size = ImGui::GetWindowSize();
		//static ImVec2 size(300, 665);
		//ImGui::SetWindowSize(size);
		//ImGui::Text("WinSize %f %f", size.x, size.y);
		////ImGui::Text("Pos %f %f", ImGui::GetWindowPos().x, ImGui::GetWindowPos().y);
		////ImGui::SetWindowPos(ImVec2(0, -18));

		////ImGui::InvisibleButton("##dummy", size);
		//ImGui::InvisibleButton("##dummy", ImVec2(100, 100));
		//static bool check = false;
		//if (check == false)
		//	ImGui::Text("Test");
		//else
		//	ImGui::TextColored(ImVec4(1, 0, 0, 1), "Test");
		//// 여기서 item ImGui 바로 위에 선언된 걸 의미 현재는 Test Text임
		//static ImVec2 itemRectMin;
		//static ImVec2 itemRectMax;
		//static ImVec2 itemPos;
		//if (ImGui::IsItemClicked())
		//{
		//	//ImGui::CalcItemRectClosestPoint(itemPos);
		//	itemRectMin = ImGui::GetItemRectMin();
		//	itemRectMax = ImGui::GetItemRectMax();

		//	check = true;

		//	int a = 10;
		//}

		//if (check) {
		//	ImGui::Text("item pos %f %f", itemRectMin.x, itemRectMin.y);
		//	ImGui::Text("item size %f %f", itemRectMax.x, itemRectMax.y);

		//	//ImGui::GetWindowDrawList()->AddRectFilled(
		//	//	ImVec2(winPos.x + 150, winPos.y + 150), 
		//	//	ImVec2(winPos.x + 200, winPos.y + 200), IM_COL32(90, 90, 120, 255));
		//	ImGui::GetWindowDrawList()->AddRectFilled(
		//		ImVec2(itemRectMin.x, itemRectMin.y),
		//		ImVec2(itemRectMax.x, itemRectMax.y), IM_COL32(90, 90, 120, 255));
		//}


		//// clicked이랑 비슷한거 같음
		////if (ImGui::IsItemActive())
		////{
		////	int a = 10;
		////}

		////if (ImGui::IsItemActive() && ImGui::IsMouseDragging())
		////{
		////	
		////	//offset.x += ImGui::GetIO().MouseDelta.x;
		////	//offset.y += ImGui::GetIO().MouseDelta.y;
		////}

		////ImGui::Text("offset : %f, %f", offset.x, offset.y);

		//ImGui::End();
	}

	// Draw Test
	{
		//ImGui::Begin("Draw");

		//ImVec2 winPos = ImGui::GetWindowPos();
		//ImGui::GetWindowDrawList()->AddCircleFilled(
		//	ImVec2(winPos.x + 100, winPos.y + 100), 10, IM_COL32(128, 128, 128, 255));
		//ImGui::GetWindowDrawList()->AddRectFilled(
		//	ImVec2(winPos.x + 150, winPos.y + 150), ImVec2(winPos.x + 200, winPos.y + 200),
		//	IM_COL32(255, 0, 0, 255));
		//ImGui::GetWindowDrawList()->AddRect(
		//	ImVec2(winPos.x + 250, winPos.y + 250), ImVec2(winPos.x + 300, winPos.y + 300),
		//	IM_COL32(0, 255, 0, 255));

		//ImGui::End();
	}

}

