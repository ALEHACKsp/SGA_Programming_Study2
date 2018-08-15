#pragma once

#include "GameModel.h"

class GameBullet : public GameModel
{
public:
	GameBullet(wstring matFolder, wstring meshFolder, wstring name);
	~GameBullet();

	void Update() override;
	void Render() override;

	void PostRender() override;

	bool IsCollision(GameModel * model) override;
	void * GetCollider() override { return collider; }

	void Fire(D3DXVECTOR3& firePos, D3DXVECTOR3& velocity);
	void Move();

	void SetFire(bool isFire) { this->isFire = isFire; }
	bool GetFire() { return isFire; }

	D3DXVECTOR3 FirePosition() { return firePosition; }

private:
	class RectCollider * collider;

	D3DXVECTOR3 firePosition;

	bool isFire;
};

class GameBullets
{
public:
	GameBullets(wstring matFolder, wstring meshFolder, wstring name,
		int bulletMax, float range);
	~GameBullets();

	void Update();
	void Render();

	void PostRender();

	bool IsCollision(GameModel * model);

	void Fire(D3DXVECTOR3& firePos, D3DXVECTOR3& velocity);

private:
	vector<GameBullet*> bullets;

	float range;
};