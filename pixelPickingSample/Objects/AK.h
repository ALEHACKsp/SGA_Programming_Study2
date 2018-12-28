#pragma once
#include "GameModel.h"

class AK : public GameModel
{
public:
	AK();
	~AK();

	void Update() override;
	void Render() override;
private:
};