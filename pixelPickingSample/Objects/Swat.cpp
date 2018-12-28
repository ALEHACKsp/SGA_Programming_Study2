#include "stdafx.h"
#include "Swat.h"
#include "./Physics/CapsuleCollider.h"
#include "./Model/ModelClip.h"
#include "./Objects/AK.h"

Swat::Swat()
	: GameAnimModel(Models + L"Swat/", L"Swat.material", Models + L"Swat/", L"Swat.mesh")
	, state(State::Idle), preState(State::Idle)
	, damage(50.0f), hp(100.0f)
{
	SetShader(Shaders + L"035_Animation.hlsl");

	RootAxis(D3DXVECTOR3(0, 180.0f, 0));
	Scale(D3DXVECTOR3(0.1f, 0.1f, 0.1f));

	collider = new CapsuleCollider(28.0f, 54.0f, this->GetTransform());
	CapsuleCollider* capCol = reinterpret_cast<CapsuleCollider*>(collider);

	ak = new AK();
	ak->LinkWorld(&handBone);
	ak->Scale(0.35f, 0.35f, 0.35f);
	ak->RotationDegree(0, 100.0f, -90.0f);
	ak->Position(-35.0f, -2.0f, 4.0f);

	AddClip(Models + L"Swat/Idle.anim");
	AddClip(Models + L"Swat/RunForward.anim");
	AddClip(Models + L"Swat/RunForwardLeft.anim");
	AddClip(Models + L"Swat/RunForwardRight.anim");
	AddClip(Models + L"Swat/RunLeft.anim");
	AddClip(Models + L"Swat/RunRight.anim");
	AddClip(Models + L"Swat/RunBackward.anim");
	AddClip(Models + L"Swat/RunBackwardLeft.anim");
	AddClip(Models + L"Swat/RunBackwardRight.anim");
	AddClip(Models + L"Swat/FiringRifle.anim");
	AddClip(Models + L"Swat/Reload.anim");
	AddClip(Models + L"Swat/HitReaction.anim");
	AddClip(Models + L"Swat/Dying.anim");
	Play((UINT)State::Idle, true, 0.0f, 15.0f);
}

Swat::~Swat()
{
	SAFE_DELETE(ak);
	SAFE_DELETE(collider);
}

void Swat::Update()
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
			switch (moveState)
			{
				case MoveState::RunForward:
					{
						if (preState != State::Move || preMoveState != MoveState::RunForward)
						{
							Play((UINT)MoveState::RunForward, true, 20.0f, 15.0f);
							preMoveState = MoveState::RunForward;
							preState = State::Move;
						}
						break;
					}
				case MoveState::RunForwardLeft:
				{
					if (preState != State::Move || preMoveState != MoveState::RunForwardLeft)
					{
						Play((UINT)MoveState::RunForwardLeft, true, 20.0f, 15.0f);
						preMoveState = MoveState::RunForwardLeft;
						preState = State::Move;
					}
					break;
				}
				case MoveState::RunForwardRight:
				{
					if (preState != State::Move || preMoveState != MoveState::RunForwardRight)
					{
						Play((UINT)MoveState::RunForwardRight, true, 20.0f, 15.0f);
						preMoveState = MoveState::RunForwardRight;
						preState = State::Move;
					}
					break;
				}
				case MoveState::RunLeft:
				{
					if (preState != State::Move || preMoveState != MoveState::RunLeft)
					{
						Play((UINT)MoveState::RunLeft, true, 20.0f, 15.0f);
						preMoveState = MoveState::RunLeft;
						preState = State::Move;
					}
					break;
				}
				case MoveState::RunRight:
				{
					if (preState != State::Move || preMoveState != MoveState::RunRight)
					{
						Play((UINT)MoveState::RunRight, true, 20.0f, 15.0f);
						preMoveState = MoveState::RunRight;
						preState = State::Move;
					}
					break;
				}
				case MoveState::RunBackward:
				{
					if (preState != State::Move || preMoveState != MoveState::RunBackward)
					{
						Play((UINT)MoveState::RunBackward, true, 20.0f, 15.0f);
						preMoveState = MoveState::RunBackward;
						preState = State::Move;
					}
					break;
				}
				case MoveState::RunBackwardLeft:
				{
					if (preState != State::Move || preMoveState != MoveState::RunBackwardLeft)
					{
						Play((UINT)MoveState::RunBackwardLeft, true, 20.0f, 15.0f);
						preMoveState = MoveState::RunBackwardLeft;
						preState = State::Move;
					}
					break;
				}
				case MoveState::RunBackwardRight:
				{
					if (preState != State::Move || preMoveState != MoveState::RunBackwardRight)
					{
						Play((UINT)MoveState::RunBackwardRight, true, 20.0f, 15.0f);
						preMoveState = MoveState::RunBackwardRight;
						preState = State::Move;
					}
					break;
				}
			}
			break;
		}
		case State::FiringRifle:
		{
			if (preState != State::FiringRifle)
			{
				Play((UINT)State::FiringRifle, false, 20.0f, 15.0f);
				preState = State::FiringRifle;
			}
			else if (GetClip((UINT)State::FiringRifle)->CheckLastFrame())
			{
				state = State::Idle;
			}
			break;
		}
		case State::Reload:
		{
			if (preState != State::Reload)
			{
				Play((UINT)State::Reload, false, 20.0f, 15.0f);
				preState = State::Reload;
			}
			else if (GetClip((UINT)State::Reload)->CheckLastFrame())
			{
				state = State::Idle;
			}
			break;
		}
		case State::HitReaction:
		{
			if (preState != State::HitReaction)
			{
				if (hp < 0.0f)
				{
					state = State::Dying;
					break;
				}
				Play((UINT)State::HitReaction, false, 1.0f, 15.0f);
				preState = State::HitReaction;
				SetDiffuse(1, 0, 0);
			}
			else if (GetClip((UINT)State::HitReaction)->CheckLastFrame())
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
	GetBoneTransformByName(L"swat:RightHand", &handBone);
	ak->Update();
}

void Swat::Render()
{
	__super::Render();
	ak->Render();
}

Collider * Swat::GetRayCollider()
{
	return ak->GetCollider();
}
