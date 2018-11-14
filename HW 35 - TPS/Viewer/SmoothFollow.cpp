#include "stdafx.h"
#include "SmoothFollow.h"

SmoothFollow::SmoothFollow(float distance, float height, float heightDamping, float rotationDamping)
	: distance(distance), height(height), heightDamping(heightDamping), rotationDamping(rotationDamping)
{
}

SmoothFollow::~SmoothFollow()
{
}

void SmoothFollow::Update()
{
	D3DXVECTOR3 position, forward;
	D3DXVECTOR2 rotation;
	Position(&position);
	forward = D3DXVECTOR3(0, 0, 1);
	Rotation(&rotation);
		
	// Calculate the current rotation angles
	float wantedRotationAngle = targetRot.y;
	float wantedHeight = targetPos.y + height;

	//float currentRotationAngle = targetRot.y;
	float currentRotationAngle = rotation.y;
	float currentHeight = position.y;

	// Damp the rotation around the y-axis
	currentRotationAngle = Math::LerpAngle(currentRotationAngle, wantedRotationAngle, rotationDamping * Time::Delta());
	//currentRotationAngle = Math::Lerp(currentRotationAngle, wantedRotationAngle, rotationDamping * Time::Delta());

	// Damp the Height
	currentHeight = Math::Lerp(currentHeight, wantedHeight, heightDamping * Time::Delta());

	// Convert the angle into a rotation
	D3DXMATRIX rotMatrix;
	D3DXMatrixRotationY(&rotMatrix, currentRotationAngle);
	D3DXQUATERNION currentRotation;
	//D3DXQuaternionRotationYawPitchRoll(&currentRotation, currentRotationAngle, 0, 0);
	D3DXQuaternionRotationMatrix(&currentRotation, &rotMatrix);

	// Set the position of the camera on the x-z plane to:
	// distance meters behind the target
	position = targetPos;
	//position -= Math::D3DXVec3Rotate(&forward, &currentRotation) * distance;
	//position -= Math::MultiplyQuatVec3(currentRotation, forward) * distance;
	position -= Math::RotateVectorByQuaternion(forward, currentRotation) * distance;
	
	position = D3DXVECTOR3(position.x, currentHeight, position.z);

	D3DXVECTOR3 lookAt = targetPos - position;
	D3DXQUATERNION quat = Math::LookAt(lookAt, targetUp);
	D3DXVECTOR3 rot;
	Math::toEulerAngle(quat, rot);

	rotation.x = rot.x;
	rotation.y = rot.y;

	Position(position.x, position.y, position.z);
	Rotation(rotation.x, rotation.y);
	View();
}

void SmoothFollow::SetTarget(D3DXVECTOR3& targetPos, D3DXVECTOR3& targetRot, D3DXVECTOR3& targetUp)
{
	this->targetPos = targetPos;
	this->targetRot = targetRot + D3DXVECTOR3(0, D3DX_PI, 0);
	this->targetRot.y = Math::NormalizeAngle(this->targetRot.y);
	this->targetUp = targetUp;
	D3DXVec3Normalize(&this->targetUp, &this->targetUp);
}

void SmoothFollow::PostRender()
{
	ImGui::Begin("Smooth Follow");

	ImGui::DragFloat("Distance", &distance, 0.1f);
	ImGui::DragFloat("Height", &height, 0.1f);
	
	ImGui::DragFloat("HeightDamping", &heightDamping, 0.1f);
	ImGui::DragFloat("RotationDamping", &rotationDamping, 0.1f);

	ImGui::End();
}
