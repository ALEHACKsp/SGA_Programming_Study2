#pragma once

#include "GameModel.h"

class GameTank : public GameModel
{
public:
	GameTank(wstring matFolder, wstring meshFolder, wstring name);
	~GameTank();

	void Update() override;
	void Render() override;

	bool IsCollision(GameModel * model) override;
	void * GetCollider() override { return collider; }

private:
	class RectCollider* collider;
};