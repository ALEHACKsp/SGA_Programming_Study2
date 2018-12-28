#include "stdafx.h"
#include "ExePicking.h"
#include "../Landscape/Terrain.h"
#include "../Objects/Paladin.h"
#include "./Utilities/Gizmo.h"
#include "./Physics/Collider.h"
#include "./Objects/PickingObject.h"

ExePicking::ExePicking(ExecuteValues * values)
	: Execute(values), pickedObj(NULL), pickedPart(NULL)
{
	{
		Paladin* tank = new Paladin();
		tank->Update();
		objects.push_back(tank);
	}

	//picking
	objPicking = new PickingObject(values);

}

ExePicking::~ExePicking()
{
	SAFE_DELETE(objPicking);

	for (GameModel* object : objects)
		SAFE_DELETE(object);
}

void ExePicking::Update()
{
	if (Mouse::Get()->Up(0))
	{
		if (objPicking->Picking(objects)) {
			pickedObj = objPicking->GetPickedObj();
		}
		else
		{
			objPicking->UnPicking();
		}
	}
	for (GameModel* object : objects)
		object->Update();
}

void ExePicking::PreRender()
{
}

void ExePicking::Render()
{
	for (GameModel* object : objects)
		object->Render();
}

void ExePicking::PostRender()
{
	if (pickedPart != NULL)
	{
		D3DXVECTOR3 position(partTransform._41, partTransform._42, partTransform._43);
		ImGui::SliderFloat3("Position", position, -500.0f, 500.0f);
		partTransform._41 = position.x;
		partTransform._42 = position.y;
		partTransform._43 = position.z;
		ImGui::SliderFloat2("Rotation", partRot, -8.0f, 8.0f);

		D3DXMATRIX X, Y;
		D3DXMatrixRotationX(&X, partRot.x);
		D3DXMatrixRotationY(&Y, partRot.y);
		pickedPart->Local(X * Y * partTransform);
		pickedObj->Update();
	}
}

void ExePicking::ResizeScreen()
{
}
