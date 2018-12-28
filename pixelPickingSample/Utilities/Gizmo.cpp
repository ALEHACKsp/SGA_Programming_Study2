#include "stdafx.h"
#include "Gizmo.h"
#include "./Renders/LineDraw.h"
#include "./Objects/GameModel.h"
#include "./Physics/Collider.h"

Gizmo::Gizmo()
	: bOn(true)
{	
}

Gizmo::~Gizmo()
{
	for (iter = gizmos.begin(); iter != gizmos.end(); ++iter)
		SAFE_DELETE(iter->second);
}

void Gizmo::Update()
{
	if (bOn)
	{
		for (iter = gizmos.begin(); iter != gizmos.end(); ++iter)
			iter->second->Update();
	}
}

void Gizmo::Update(D3DXMATRIX& transform)
{
	if (bOn)
	{
		for (iter = gizmos.begin(); iter != gizmos.end(); ++iter)
			iter->second->Update(transform);
	}
}

void Gizmo::Render()
{
	if (bOn)
	{
		for (iter = gizmos.begin(); iter != gizmos.end(); ++iter)
			iter->second->Render();
	}
}

void Gizmo::DrawGizmoDirection(GameModel * model, D3DXCOLOR color)
{
	D3DXMATRIX local = model->RootAxis();
	D3DXVECTOR3 direction;
	D3DXVec3Normalize(&direction, &D3DXVECTOR3(local._31, local._32, local._33));

	D3DXVECTOR3 minBox, maxBox;
	model->GetModel()->BoundingBox(&minBox, &maxBox);
	D3DXVECTOR3 boxLength = maxBox - minBox;
	float radius = max(max(boxLength.x, boxLength.y), boxLength.z);

	iter = gizmos.find("Direction");
	if (iter == gizmos.end())
	{
		LineDraw* gizmo = new LineDraw(model->WorldLink());
		gizmo->DrawLine(D3DXVECTOR3(0, 0, 0), direction * radius * 2, color);
		gizmos.insert(Pair("Direction", gizmo));
	}
	else
	{
		iter->second->DrawLine(D3DXVECTOR3(0, 0, 0), direction * radius * 2, color);
	}
}

void Gizmo::DrawGizmoRight(GameModel * model, D3DXCOLOR color)
{
	D3DXMATRIX local = model->RootAxis();
	D3DXVECTOR3 right;
	D3DXVec3Normalize(&right, &D3DXVECTOR3(local._11, local._12, local._13));

	D3DXVECTOR3 minBox, maxBox;
	model->GetModel()->BoundingBox(&minBox, &maxBox);
	D3DXVECTOR3 boxLength = maxBox - minBox;
	float radius = max(max(boxLength.x, boxLength.y), boxLength.z);

	iter = gizmos.find("Right");
	if (iter == gizmos.end())
	{
		LineDraw* gizmo = new LineDraw(model->WorldLink());
		gizmo->DrawLine(D3DXVECTOR3(0, 0, 0), right * radius * 2, color);
		gizmos.insert(Pair("Right", gizmo));
	}
	else
	{
		iter->second->DrawLine(D3DXVECTOR3(0, 0, 0), right * radius * 2, color);
	}
}

void Gizmo::DrawGizmoUp(GameModel * model, D3DXCOLOR color)
{
	D3DXMATRIX local = model->RootAxis();
	D3DXVECTOR3 up;
	D3DXVec3Normalize(&up, &D3DXVECTOR3(local._21, local._22, local._23));

	D3DXVECTOR3 minBox, maxBox;
	model->GetModel()->BoundingBox(&minBox, &maxBox);
	D3DXVECTOR3 boxLength = maxBox - minBox;
	float radius = max(max(boxLength.x, boxLength.y), boxLength.z);

	iter = gizmos.find("Up");
	if (iter == gizmos.end())
	{
		LineDraw* gizmo = new LineDraw(model->WorldLink());
		gizmo->DrawLine(D3DXVECTOR3(0, 0, 0), up * radius * 2, color);
		gizmos.insert(Pair("Up", gizmo));
	}
	else
	{
		iter->second->DrawLine(D3DXVECTOR3(0, 0, 0), up * radius * 2, color);
	}
}

void Gizmo::DrawGizmoRay(string name, D3DXMATRIX * transform, D3DXVECTOR3 & direction, D3DXCOLOR color)
{
	iter = gizmos.find(name);
	if (iter == gizmos.end())
	{
		LineDraw* gizmo = new LineDraw(transform);
		gizmo->DrawLine(D3DXVECTOR3(0, 0, 0), direction * 800, color);
		gizmos.insert(Pair(name, gizmo));
	}
	else
	{
		iter->second->DrawLine(D3DXVECTOR3(0, 0, 0), direction * 800, color);
	}
}

void Gizmo::DrawGizmoBox(string name, D3DXMATRIX* transform, D3DXVECTOR3& minPos, D3DXVECTOR3& maxPos, D3DXCOLOR color)
{
	iter = gizmos.find(name);
	if (iter == gizmos.end())
	{
		LineDraw* gizmo = new LineDraw(transform);
		gizmo->DrawBox(minPos, maxPos, color);
		gizmos.insert(Pair(name, gizmo));
	}
	else
	{
		iter->second->DrawBox(minPos, maxPos, color);
	}
}

void Gizmo::DrawGizmoSphere(string name, D3DXMATRIX * transform, float & radius, D3DXVECTOR3 center, D3DXCOLOR color)
{
	iter = gizmos.find(name);
	if (iter == gizmos.end())
	{
		LineDraw* gizmo = new LineDraw(transform);
		gizmo->DrawSphere(center, radius, color);
		gizmos.insert(Pair(name, gizmo));
	}
	else
	{
		iter->second->DrawSphere(center, radius, color);
	}
}

void Gizmo::DrawGizmoCapsule(string name, D3DXMATRIX * transform, D3DXVECTOR3 & point1, D3DXVECTOR3 & point2, float & radius, D3DXCOLOR color)
{
	iter = gizmos.find(name);
	if (iter == gizmos.end())
	{
		LineDraw* gizmo = new LineDraw(transform);
		gizmo->DrawCapsule(point1, point2, radius, color);
		gizmos.insert(Pair(name, gizmo));
	}
	else
	{
		iter->second->DrawCapsule(point1, point2, radius, color);
	}
}
