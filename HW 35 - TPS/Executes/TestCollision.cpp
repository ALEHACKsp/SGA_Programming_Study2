#include "stdafx.h"
#include "TestCollision.h"

#include "Physics\SphereCollider.h"
#include "Physics\CapsuleCollider.h"
#include "Physics\Ray.h"

TestCollision::TestCollision(ExecuteValues * values)
	:Execute(values)
{
	origin = D3DXVECTOR3(0, 0, 0);
	direction = D3DXVECTOR3(0, 0, 1);
	ray = new Ray(origin, direction, 1000, true);

	capsule = new CapsuleCollider();
	capsule->Scale(5, 5, 5);
	capsule->Position(0, 0, 100);

	sphere = new SphereCollider();
	sphere->Scale(5, 5, 5);
	sphere->Position(-100, 0, 100);

	bSphereCollision = false;
	bCapsuleCollision = false;
}

TestCollision::~TestCollision()
{
	SAFE_DELETE(ray);
	SAFE_DELETE(capsule);
	SAFE_DELETE(sphere);
}

void TestCollision::Update()
{
	capsule->Update();
	sphere->Update();

	// Ray Capsule 및 Sphere 충돌 방법 2가지 2번째 방법은 더 간단
	// Collision 클래스쪽도 정의 같이 주석 or 주석해제 해줘야함
	#define EFFICIENT_RAY_COLLISION

#ifndef EFFICIENT_RAY_COLLISION
	float min, max;
	bSphereCollision = Collision::IsOverlapRaySphere(ray, sphere, &min, &max);
	D3DXVECTOR3 p1, p2, n1, n2;
	bCapsuleCollision = Collision::ISOverlapRayCapsule(ray, capsule, &p1, &p2, &n1, &n2);
#else
	bSphereCollision = Collision::IsOverlapRaySphere(ray, sphere);
	bCapsuleCollision = Collision::IsOverlayRayCapsule(ray, capsule);
#endif // !EFFICIENT_RAY_COLLISION

}

void TestCollision::PreRender()
{

}

void TestCollision::Render()
{
	ray->Render();
	capsule->Render();
	sphere->Render();
}

void TestCollision::PostRender()
{
	ImGui::Begin("Physics");
	ImGui::Text("Ray");
	ray->PostRender();
	ImGui::Separator();

	ImGui::Text("Capsule");
	capsule->PostRender();
	ImGui::Separator();

	ImGui::Text("Sphere");
	sphere->PostRender();
	ImGui::Separator();

	ImGui::Text("Ray Sphere Collision %s", bSphereCollision ? "True" : "False");
	ImGui::Text("Ray Capsule Collision %s", bCapsuleCollision ? "True" : "False");

	ImGui::End();
}
