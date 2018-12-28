#pragma once

class Manifold
{
public:
	static bool Solve(Collider* a, Collider* b, float * dist = NULL);
	static bool Solve(GameModel* a, GameModel* b, float * dist = NULL);
};