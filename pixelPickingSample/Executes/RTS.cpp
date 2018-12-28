#include "stdafx.h"
#include "RTS.h"
#include "./Viewer/Orbit.h"
#include "./Objects/MeshBunny.h"
#include "./Objects/Swat.h"
#include "./Objects/Vanguard.h"
#include "./Landscape/Sky.h"

RTS::RTS(ExecuteValues * values)
	: Execute(values)
{
	swat = new Swat();
	SAFE_DELETE(values->MainCamera);
	values->MainCamera = new Orbit(&camPos, 30.0f, 10.0f, rotationSpeed);
	reinterpret_cast<Orbit*>(values->MainCamera)->SetFocusPoint(D3DXVECTOR3(0, 15, 0));

	sky = new Sky(values);

	for (int i = 0; i < 5; i++)
	{
		Vanguard* vanguard = new Vanguard();
		vanguard->Position(i * 50.0f, 0, 50.0f);
		monsters.push_back(vanguard);
	}
	Mouse::Get()->CursorLoop(true);
	ShowCursor(false);
	ShowCursor(false);
}

RTS::~RTS()
{
	SAFE_DELETE(sky);
	SAFE_DELETE(swat);

	for (GameModel* monster : monsters)
		SAFE_DELETE(monster);

}

void RTS::Update()
{
	sky->Update();

	SwatCommend();
	swat->Update();

	for (GameModel* monster : monsters)
		monster->Update();

	//카메라 위치 보정
	camPos = swat->Position();
	D3DXVECTOR3 right;
	D3DXVec3Normalize(&right, &swat->Right());
	camPos += right * 5;
}

void RTS::PreRender()
{
	sky->PreRender();
}

void RTS::Render()
{
	sky->Render();
	
	for (GameModel* monster : monsters)
		monster->Render();

	swat->Render();

	bool collision = false;
	float dist = INFINITY;

	for (GameModel* monster : monsters)
	{
		float distTemp = 0;
		if (Manifold::Solve(swat->GetRayCollider(), monster->GetCollider(), &distTemp))
		{
			if (swat->GetState() == State::FiringRifle)
			{
				if (reinterpret_cast<Vanguard*>(monster)->GetState() == MonsterState::Hit) continue;
				reinterpret_cast<Vanguard*>(monster)->SetState(MonsterState::Hit);
				reinterpret_cast<Vanguard*>(monster)->Damaged(50);
			}
			collision = true;
		}
	}
	
	ImGui::Begin("Collider");
	{
		ImGui::Text("Collision %d", int(collision));
	}
	ImGui::End();
}

void RTS::PostRender()
{	
}

void RTS::SwatCommend()
{
	//	회전
	{
		D3DXVECTOR2 rotation = swat->Rotation();

		D3DXVECTOR3 val = Mouse::Get()->GetMoveValue();

		rotation.y += val.x * rotationSpeed * Time::Delta();

		swat->Rotation(D3DXVECTOR3(0.0f, rotation.y, 0.0f));
	}

	if (swat->GetState() == State::HitReaction ||
		swat->GetState() == State::Dying )
		return;
	
	forward = Keyboard::Get()->Press('W');
	backward = Keyboard::Get()->Press('S');
	left = Keyboard::Get()->Press('A');
	right = Keyboard::Get()->Press('D');

	if (forward || backward || left || right)
	{
		if(swat->GetState() != State::FiringRifle && swat->GetState() != State::Reload)
			swat->SetState(State::Move);

		if (forward)
		{
			swat->SetMoveState(MoveState::RunForward);
			swat->MoveToDirectionDelta(-moveSpeed, Move::Direction, 0);
		}
		else if (backward)
		{
			swat->SetMoveState(MoveState::RunBackward);
			swat->MoveToDirectionDelta(moveSpeed, Move::Direction, 0);
		}
		if (left)
		{
			swat->SetMoveState(MoveState::RunLeft);
			swat->MoveToDirectionDelta(moveSpeed, Move::Right, 0);
		}
		else if (right)
		{
			swat->SetMoveState(MoveState::RunRight);
			swat->MoveToDirectionDelta(-moveSpeed, Move::Right, 0);
		}

		if (forward && left)
			swat->SetMoveState(MoveState::RunForwardLeft);
		else if (forward && right)
			swat->SetMoveState(MoveState::RunForwardRight);
		if (backward && left)
			swat->SetMoveState(MoveState::RunBackwardLeft);
		else if (backward && right)
			swat->SetMoveState(MoveState::RunBackwardRight);
	}
	else
	{
		if (swat->GetState() != State::FiringRifle && swat->GetState() != State::Reload)
			swat->SetState(State::Idle);
	}

	if (Mouse::Get()->Down(0))
	{
		swat->SetState(State::FiringRifle);
	}

	if (Keyboard::Get()->Press('R'))
	{
		swat->SetState(State::Reload);
	}
}
