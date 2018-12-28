#include "stdafx.h"
#include "Vanguard.h"
#include "./Physics/CapsuleCollider.h"
#include "./Model/ModelClip.h"
#include "./Utilities/Timer.h"

Vanguard::Vanguard()
	: GameAnimModel(Models + L"Vanguard/", L"Vanguard.material", Models + L"Vanguard/", L"Vanguard.mesh")
	, state(MonsterState::Idle), preState(MonsterState::Idle)
	, hp(100.0f), attackRange(10.0f), moveRange(30.0f), coolTime(5.0f), bLive(true), damage(10.0f)
{
	shader = new Shader(Shaders + L"035_Animation.hlsl");
	SetShader(shader);

	RootAxis(D3DXVECTOR3(0, 180.0f, 0));
	Scale(D3DXVECTOR3(0.1f, 0.1f, 0.1f));

	collider = new CapsuleCollider(28.0f, 54.0f, this->GetTransform());
	CapsuleCollider* capCol = reinterpret_cast<CapsuleCollider*>(collider);

	//handCollider = new CapsuleCollider(&handBone, 7.0f, 30.0f);

	timer = new Timer();

	AddClip(Models + L"Vanguard/Idle.anim");
	AddClip(Models + L"Vanguard/Walking.anim");
	AddClip(Models + L"Vanguard/Punch.anim");
	AddClip(Models + L"Vanguard/Hit.anim");
	AddClip(Models + L"Vanguard/Dying.anim");
	Play((UINT)MonsterState::Idle, true, 0.0f, 15.0f);
}

Vanguard::~Vanguard()
{
	SAFE_DELETE(timer);
	SAFE_DELETE(handCollider);
	SAFE_DELETE(collider);
}

void Vanguard::Update()
{
	if (bLive == false)
		return;

	switch (state)
	{
		case MonsterState::Idle:
		{
			if (preState != MonsterState::Idle)
			{
				Play((UINT)MonsterState::Idle, true, 20.0f, 15.0f);
				preState = MonsterState::Idle;
			}
			break;
		}
		case MonsterState::Move:
		{
			if (preState != MonsterState::Move)
			{
				Play((UINT)MonsterState::Move, true, 20.0f, 15.0f);
				preState = MonsterState::Move;
			}
			break;
		}
		case MonsterState::Attack:
		{
			if (preState != MonsterState::Attack)
			{
				if (timer->CoolTime(coolTime) == false)
				{
					state = MonsterState::Idle;
					break;
				}
				Play((UINT)MonsterState::Attack, false, 20.0f, 15.0f);
				preState = MonsterState::Attack;
			}
			else if (GetClip((UINT)MonsterState::Attack)->CheckLastFrame())
			{
				state = MonsterState::Idle;
				Play((UINT)MonsterState::Idle, true, 20.0f, 15.0f);
				preState = MonsterState::Idle;
			}
			break;
		}
		case MonsterState::Hit:
		{
			if (preState != MonsterState::Hit)
			{
				if (hp < 0.0f)
				{
					state = MonsterState::Dying;
					break;
				}
				Play((UINT)MonsterState::Hit, false, 1.0f, 15.0f);
				preState = MonsterState::Hit;
				SetDiffuse(1, 0, 0);
			}
			else if (GetClip((UINT)MonsterState::Hit)->CheckLastFrame())
			{
				state = MonsterState::Idle;
				Play((UINT)MonsterState::Idle, true, 20.0f, 15.0f);
				preState = MonsterState::Idle;
				SetDiffuse(1, 1, 1);
			}
			break;
		}
		case MonsterState::Dying:
		{
			if (preState != MonsterState::Dying)
			{
				Play((UINT)MonsterState::Dying, false, 20.0f, 15.0f);
				preState = MonsterState::Dying;
				SetDiffuse(1, 0, 0);
			}
			else if (GetClip((UINT)MonsterState::Dying)->CheckLastFrame())
			{
				state = MonsterState::Idle;
				Play((UINT)MonsterState::Idle, true, 20.0f, 15.0f);
				preState = MonsterState::Idle;
				bLive = false;
				SetDiffuse(1, 1, 1);
			}
			break;
		}
	}
	__super::Update();
	GetBoneTransformByName(L"mixamorig:RightHand", &handBone);
}

void Vanguard::Render()
{
	if (bLive == false)
		return;

	__super::Render();
}