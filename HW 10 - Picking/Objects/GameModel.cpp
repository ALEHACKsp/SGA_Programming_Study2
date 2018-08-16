#include "stdafx.h"
#include "GameModel.h"

#include "../Viewer/Perspective.h"
#include "../Viewer/Viewport.h"
#include "../Viewer/Camera.h"

#include "../Physics/RectCollider.h"

#include "../Renders/Line.h"

GameModel::GameModel(wstring matFolder, wstring matFile, wstring meshFolder, wstring meshFile)
	: velocity(0,0,0)
{
	model = new Model();
	model->ReadMaterial(matFolder, matFile);
	model->ReadMesh(meshFolder, meshFile);

	D3DXMatrixIdentity(&matRotation);

	renderBuffer = new RenderBuffer();

	//turretBone = model->BoneByName(L"turret_geo");

	line = new Line();
}

GameModel::~GameModel()
{
	SAFE_DELETE(renderBuffer);

	SAFE_DELETE(model);

	SAFE_DELETE(line);
}

void GameModel::Velocity(D3DXVECTOR3 & vec)
{
	velocity = vec;
}

D3DXVECTOR3 GameModel::Velocity()
{
	return velocity;
}

void GameModel::Update()
{
	CalcPosition();

	//D3DXMATRIX R;
	//D3DXMatrixRotationY(&R, sinf(Time::Get()->Running() * 2.0f) *0.25f);
	//
	//D3DXMATRIX L = turretBone->Local();
	//turretBone->Local(L * R);

	D3DXMATRIX t = Transformed();
	// 부모부터 자식까지 쫙 해주는거
	model->CopyGlobalBoneTo(boneTransforms, t);
}

void GameModel::Render()
{
	model->Buffer()->Bones(&boneTransforms[0], boneTransforms.size());
	model->Buffer()->SetVSBuffer(2);

	for (ModelMesh* mesh : model->Meshes())
	{
		// bone index
		int index = mesh->ParentBoneIndex();

		renderBuffer->Data.Index = index;
		renderBuffer->SetVSBuffer(3);

		mesh->Render();
	}
}

void GameModel::Rotate(D3DXVECTOR2 amount)
{
	amount *= Time::Delta();

	// x, y 뒤집어둠 카메라 움직임때랑 같은 이유
	D3DXMATRIX axis;
	D3DXMatrixRotationAxis(&axis, &Right(), amount.y);

	D3DXMATRIX Y;
	D3DXMatrixRotationY(&Y, Math::ToRadian(amount.x));

	matRotation = axis * Y;

	D3DXMATRIX R = World();
	World(matRotation * R);
}

bool GameModel::IsMouseCollision(Camera * camera, Viewport * viewport, Perspective * perspective)
{
	if (colliderType == COLLIDER_TYPE_RECT)
	{
		RectCollider * collider = (RectCollider*)GetCollider();
		
		D3DXVECTOR3 p[8];
		D3DXVECTOR3 minPos, maxPos;
		collider->GetWorldAABBMinMax(&Transformed(), &minPos, &maxPos);

		p[0] = { minPos.x, minPos.y, minPos.z };
		p[1] = { minPos.x, maxPos.y, minPos.z };
		p[2] = { maxPos.x, minPos.y, minPos.z };
		p[3] = { maxPos.x, maxPos.y, minPos.z };
		p[4] = { minPos.x, minPos.y, maxPos.z };
		p[5] = { minPos.x, maxPos.y, maxPos.z };
		p[6] = { maxPos.x, minPos.y, maxPos.z };
		p[7] = { maxPos.x, maxPos.y, maxPos.z };

		bool result = false;
		bool temp;

		// 앞
		temp = Collision::IsPeek(camera, viewport, perspective,
			p[0], p[1], p[2]);
		if (temp) {
			D3DXVECTOR3 lines[] = { p[0],p[1],p[2],p[0] };
			line->Render(lines, 3);
		}
		result |= temp;

		temp = Collision::IsPeek(camera, viewport, perspective,
			p[2], p[1], p[3]);
		if (temp) {
			D3DXVECTOR3 lines[] = { p[2],p[1],p[3],p[2] };
			line->Render(lines, 3);
		}
		result |= temp;

		// 뒤
		temp = Collision::IsPeek(camera, viewport, perspective, 
			p[6], p[7], p[4]);
		if (temp) {
			D3DXVECTOR3 lines[] = { p[6],p[7],p[4],p[6] };
			line->Render(lines, 3);
		}
		result |= temp;

		temp = Collision::IsPeek(camera, viewport, perspective, 
			p[4], p[7], p[5]);
		if (temp) {
			D3DXVECTOR3 lines[] = { p[4],p[7],p[5],p[4] };
			line->Render(lines, 3);
		}
		result |= temp;

		// 왼쪽
		temp = Collision::IsPeek(camera, viewport, perspective, 
			p[4], p[5], p[0]);
		if (temp) {
			D3DXVECTOR3 lines[] = { p[4],p[5],p[0],p[4] };
			line->Render(lines, 3);
		}
		result |= temp;

		temp = Collision::IsPeek(camera, viewport, perspective, 
			p[0], p[5], p[1]);
		if (temp) {
			D3DXVECTOR3 lines[] = { p[0],p[5],p[1],p[0] };
			line->Render(lines, 3);
		}
		result |= temp;

		// 오른쪽
		temp = Collision::IsPeek(camera, viewport, perspective, 
			p[2], p[3], p[6]);
		if (temp) {
			D3DXVECTOR3 lines[] = { p[2],p[3],p[6],p[2] };
			line->Render(lines, 3);
		}
		result |= temp;

		temp = Collision::IsPeek(camera, viewport, perspective, 
			p[6], p[3], p[7]);
		if (temp) {
			D3DXVECTOR3 lines[] = { p[6],p[3],p[7],p[6] };
			line->Render(lines, 3);
		}
		result |= temp;

		// 위
		temp = Collision::IsPeek(camera, viewport, perspective, 
			p[1], p[5], p[3]);
		if (temp) {
			D3DXVECTOR3 lines[] = { p[1],p[5],p[3],p[1] };
			line->Render(lines, 3);
		}
		result |= temp;

		temp = Collision::IsPeek(camera, viewport, perspective, 
			p[3], p[5], p[7]);
		if (temp) {
			D3DXVECTOR3 lines[] = { p[3],p[5],p[7],p[3] };
			line->Render(lines, 3);
		}
		result |= temp;

		// 아래
		temp = Collision::IsPeek(camera, viewport, perspective, 
			p[0], p[4], p[6]);
		if (temp) {
			D3DXVECTOR3 lines[] = { p[0],p[4],p[6],p[0] };
			line->Render(lines, 3);
		}
		result |= temp;

		temp = Collision::IsPeek(camera, viewport, perspective, 
			p[6], p[0], p[2]);
		if (temp) {
			D3DXVECTOR3 lines[] = { p[6],p[0],p[2],p[6] };
			line->Render(lines, 3);
		}
		result |= temp;

		return result;
	}

	return false;
}

void GameModel::CalcPosition()
{
	if (D3DXVec3Length(&velocity) <= 0.0f)
		return;


	D3DXVECTOR3 vec(0, 0, 0);
	if (velocity.z != 0.0f)
		vec += Direction() * velocity.z;

	if (velocity.y != 0.0f)
		vec += Up() * velocity.y;

	if (velocity.x != 0.0f)
		vec += Right() * velocity.x;

	D3DXVECTOR3 pos = Position() + vec * Time::Delta();
	Position(pos);
}
