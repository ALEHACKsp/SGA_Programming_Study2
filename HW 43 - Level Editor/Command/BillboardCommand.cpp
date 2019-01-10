#include "stdafx.h"
#include "BillboardCommand.h"

#include "Objects/Billboard.h"

BillboardCommand::BillboardCommand(Billboard * billboard)
	:billboard(billboard)
	, instanceCount(0)
	, id(commandId++)
{
	prevInstanceCount = billboard->GetCount();
}

BillboardCommand::~BillboardCommand()
{
}

void BillboardCommand::Execute()
{
	if (instanceCount == 0)
		instanceCount = billboard->GetCount();
	else
		billboard->SetCount(instanceCount);
}

void BillboardCommand::Undo()
{
	billboard->SetCount(prevInstanceCount);
}

void BillboardCommand::Render()
{
	ImGui::Text("Billboard #%d", id);

	ImGui::BulletText("Prev Cnt : %d", prevInstanceCount);
	ImGui::BulletText("Next Cnt : %d", instanceCount);
}
