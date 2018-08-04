#pragma once
#include "Camera.h" // 상속이나 특별한 경우만 헤더에 include 하자

class Freedom : public Camera
{
public:
	Freedom(float moveSpeed = 20.0f, float rotationSpeed = 2.5f);
	~Freedom();

	void Update();

private:
	float moveSpeed;
	float rotationSpeed;
};

