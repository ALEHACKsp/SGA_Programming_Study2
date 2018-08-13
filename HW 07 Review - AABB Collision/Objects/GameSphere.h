#pragma once

#include "GameModel.h"

class GameSphere : public GameModel
{
public:
	GameSphere(wstring matFolder, wstring meshFolder, wstring name);
	~GameSphere();

	void Update() override;
	void Render() override;
	
	bool IsCollision(GameModel * model) override;
	void * GetCollider() override { return collider; }

private:
	class CircleCollider * collider;
};