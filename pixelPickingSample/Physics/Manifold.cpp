#include "stdafx.h"
#include "Manifold.h"
#include "./Objects/GameModel.h"
#include "Collider.h"
#include "Collision.h"

bool Manifold::Solve(Collider * a, Collider * b, float * dist)
{
	return Dispatch[a->GetType()][b->GetType()](a, b, dist);
}

bool Manifold::Solve(GameModel * a, GameModel * b, float * dist)
{
	return Dispatch[a->GetCollider()->GetType()][a->GetCollider()->GetType()](a->GetCollider(), b->GetCollider(), dist);
}
