#pragma once

#include "GameModel.h"

class GameTank : public GameModel
{
public:
	GameTank(wstring matFolder, wstring meshFolder, wstring name);
	~GameTank();

	void Update() override;
	void Render() override;

	void PostRender() override;

	bool IsCollision(GameModel * model) override;
	void * GetCollider() override { return collider; }

	bool IsBulletCollision(GameModel * model);

private:
	class RectCollider* collider;

	ModelBone * wheelBones[4];
	D3DXMATRIX wheels[4];

	ModelBone * turretBone;
	D3DXMATRIX turret;

	float turretAngle;

	class GameBullets* bullets;
};