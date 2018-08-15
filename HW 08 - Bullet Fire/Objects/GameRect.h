#pragma once

#include "GameModel.h"

class GameRect : public GameModel
{
public:
	GameRect(wstring matFolder,wstring meshFolder, wstring name);
	~GameRect();

	void Update() override;
	void Render() override;

	bool IsCollision(GameModel * model) override;
	void * GetCollider() override { return collider; }

private:
	class RectCollider* collider;
};