#pragma once
#include "GameModel.h"

class Bullet : public GameModel
{
public:
	Bullet(float destroyTime = 10.0f);
	~Bullet();

	void Update() override;
	void Render() override;

	void Destroy(bool isDestroy) { this->isDestroy =  isDestroy; }
	bool Destroy() { return isDestroy; }
private:
	float fireTime;
	float destroyTime;
	bool isDestroy;
};