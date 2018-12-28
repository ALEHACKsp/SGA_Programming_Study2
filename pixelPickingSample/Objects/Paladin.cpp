#include "stdafx.h"
#include "Paladin.h"
#include "./Physics/CapsuleCollider.h"
#include "./Model/ModelClip.h"
#include "./Effects/Trail.h"

Paladin::Paladin()
	: GameAnimModel(Models + L"Paladin/", L"Paladin.material", Models + L"Paladin/", L"Paladin.mesh")
	, state(State::Idle), preState(State::Idle)
	, damage(50.0f), hp(100.0f)
{
	shader = new Shader(Shaders + L"035_Animation.hlsl");
	for (Material* material : model->Materials())
		material->SetShader(shader);

	RootAxis(D3DXVECTOR3(0, 180.0f, 0));
	Scale(D3DXVECTOR3(0.1f, 0.1f, 0.1f));

	collider = new CapsuleCollider(28.0f, 54.0f, this->GetTransform());
	CapsuleCollider* capCol = reinterpret_cast<CapsuleCollider*>(collider);

	//swordCollider = new CapsuleCollider(5.0f, 75.0f, &swordBone);

	trail = new Trail(&swordBone, D3DXVECTOR3(50.0f, -120.0f, 150.0f), D3DXVECTOR3(-80.0f, -80.0f, -80.0f), 100);

	AddClip(Models + L"Paladin/Idle.anim");
	AddClip(Models + L"Paladin/Running.anim");
	AddClip(Models + L"Paladin/Slash.anim");
	AddClip(Models + L"Paladin/Hit.anim");
	AddClip(Models + L"Paladin/Dying.anim");
	Play((UINT)State::Idle, true, 0.0f, 15.0f);
}

Paladin::~Paladin()
{
	SAFE_DELETE(trail);
	SAFE_DELETE(collider);
	SAFE_DELETE(swordCollider);
}

void Paladin::Update()
{
	switch (state)
	{
		case State::Idle:
		{
			if (preState != State::Idle)
			{
				Play((UINT)State::Idle, true, 20.0f, 15.0f);
				preState = State::Idle;
			}
			break;
		}
		case State::Move:
		{
			if (preState != State::Move)
			{
				Play((UINT)State::Move, true, 20.0f, 15.0f);
				preState = State::Move;
			}
			break;
		}
		case State::Attack:
		{
			if (preState != State::Attack)
			{
				Play((UINT)State::Attack, false, 20.0f, 15.0f);
				preState = State::Attack;
			}
			else if (GetClip((UINT)State::Attack)->CheckLastFrame())
			{
				state = State::Idle;
			}
			break;
		}
		case State::Hit:
		{
			if (preState != State::Hit)
			{
				if (hp < 0.0f)
				{
					state = State::Dying;
					break;
				}
				Play((UINT)State::Hit, false, 1.0f, 15.0f);
				preState = State::Hit;
				SetDiffuse(1, 0, 0);
			}
			else if (GetClip((UINT)State::Hit)->CheckLastFrame())
			{
				state = State::Idle;
				Play((UINT)State::Idle, true, 20.0f, 15.0f);
				preState = State::Idle;
				SetDiffuse(1, 1, 1);
			}
			break;
		}
		case State::Dying:
		{
			if (preState != State::Dying)
			{
				Play((UINT)State::Dying, false, 20.0f, 15.0f);
				preState = State::Dying;
				SetDiffuse(1, 0, 0);
			}
			else if (GetClip((UINT)State::Dying)->CheckLastFrame())
			{
				state = State::Idle;
				Play((UINT)State::Idle, true, 20.0f, 15.0f);
				preState = State::Idle;
				//TODO:게임오버
				SetDiffuse(1, 1, 1);
			}
			break;
		}
	}
	__super::Update();
	GetBoneTransformByName(L"Sword_joint", &swordBone);
	trail->Update();
}

void Paladin::Render()
{
	__super::Render();
	trail->Render();
}