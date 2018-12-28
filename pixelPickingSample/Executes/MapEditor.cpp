#include "stdafx.h"
#include "MapEditor.h"
#include "./MapTool/MapTool.h"

MapEditor::MapEditor(ExecuteValues * values)
	: Execute(values)
{
	tool = new MapTool(values);
}

MapEditor::~MapEditor()
{
	SAFE_DELETE(tool);
}

void MapEditor::Update()
{
	tool->Update();
}

void MapEditor::PreRender()
{
	tool->PreRender();
}

void MapEditor::Render()
{
	tool->Render();
}

void MapEditor::PostRender()
{
	tool->PostRender();
}
