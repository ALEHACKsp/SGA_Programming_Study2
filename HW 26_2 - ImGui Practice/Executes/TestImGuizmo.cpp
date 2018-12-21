#include "stdafx.h"
#include "TestImGuizmo.h"

TestImGuizmo::TestImGuizmo(ExecuteValues * values)
	:Execute(values)
{
	D3DXMatrixIdentity(&world);
}

TestImGuizmo::~TestImGuizmo()
{

}

void TestImGuizmo::Update()
{

}

void TestImGuizmo::PreRender()
{

}

void TestImGuizmo::Render()
{

}

void TestImGuizmo::PostRender()
{
	//ImGuizmo::BeginFrame();
}

