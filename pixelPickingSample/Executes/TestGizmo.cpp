#include "stdafx.h"
#include "TestGizmo.h"
#include "./Utilities/GizmoComponent.h"

TestGizmo::TestGizmo(ExecuteValues * values)
	: Execute(values)
{
	gizmoComponent = new GizmoComponent(values);
}

TestGizmo::~TestGizmo()
{
	SAFE_DELETE(gizmoComponent);
}

void TestGizmo::Update()
{

}

void TestGizmo::PreRender()
{
	gizmoComponent->PostRender();
}

void TestGizmo::Render()
{
	gizmoComponent->Render();
}

void TestGizmo::PostRender()
{	
}