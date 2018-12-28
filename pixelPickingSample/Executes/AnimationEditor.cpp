#include "stdafx.h"
#include "AnimationEditor.h"
#include "AnimationTool\AnimationTool.h"

AnimationEditor::AnimationEditor(ExecuteValues * values)
	: Execute(values)
{
	tool = new AnimationTool(values);
}

AnimationEditor::~AnimationEditor()
{
	SAFE_DELETE(tool);
}

void AnimationEditor::Update()
{
	tool->Update();
}

void AnimationEditor::PreRender()
{
}

void AnimationEditor::Render()
{
	tool->Render();
}

void AnimationEditor::PostRender()
{
	tool->PostRender();
}
