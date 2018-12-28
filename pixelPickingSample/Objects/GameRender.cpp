#include "stdafx.h"
#include "GameRender.h"

GameRender::GameRender()
	: enable(true), visible(true)
	, position(0, 0, 0), scale(1, 1, 1), rotation(0, 0, 0)
	, direction(0, 0, 1), up(0, 1, 0), right(1, 0, 0)
	, linkedWorld(NULL)
{
	D3DXMatrixIdentity(&rootAxis);
	D3DXMatrixIdentity(&world);
	transform = new Transform();
}

GameRender::~GameRender()
{
	SAFE_DELETE(transform);
}

void GameRender::Enable(bool val)
{
	enable = val;

	for (auto temp : Enabled)
		temp(val);
}

bool GameRender::Enable()
{
	return enable;
}

void GameRender::Visible(bool val)
{
	visible = val;

	for (auto temp : Visibled)
		temp(val);
}

bool GameRender::Visible()
{
	return visible;
}

void GameRender::RootAxis(D3DXMATRIX & matrix)
{
	rootAxis = matrix;
}

void GameRender::RootAxis(D3DXVECTOR3 & rotation)
{
	D3DXMatrixRotationYawPitchRoll
	(
		&rootAxis
		, Math::ToRadian(rotation.y)
		, Math::ToRadian(rotation.x)
		, Math::ToRadian(rotation.z)
	);
}

D3DXMATRIX GameRender::RootAxis()
{
	return rootAxis;
}

D3DXMATRIX GameRender::World()
{
	return world;
}

void GameRender::World(D3DXMATRIX & world)
{
	this->world = world;
}

D3DXMATRIX* GameRender::WorldLink()
{
	transformed = rootAxis * world;
	return &transformed;
}

void GameRender::Position(D3DXVECTOR3& vec)
{
	position = vec;

	UpdateWorld();
}

void GameRender::Position(float x, float y, float z)
{
	Position(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 GameRender::Position()
{
	return position;
}

void GameRender::Scale(D3DXVECTOR3 & vec)
{
	scale = vec;

	UpdateWorld();
}

void GameRender::Scale(float x, float y, float z)
{
	Scale(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 GameRender::Scale()
{
	return scale;
}

D3DXVECTOR3 GameRender::Direction()
{
	return direction;
}

D3DXVECTOR3 GameRender::Up()
{
	return up;
}

D3DXVECTOR3 GameRender::Right()
{
	return right;
}

void GameRender::Rotation(D3DXVECTOR3 & vec)
{
	rotation = vec;

	UpdateWorld();
}

void GameRender::Rotation(float x, float y, float z)
{
	Rotation(D3DXVECTOR3(x, y, z));
}

void GameRender::RotationDegree(D3DXVECTOR3 & vec)
{
	D3DXVECTOR3 temp;

	temp.x = Math::ToRadian(vec.x);
	temp.y = Math::ToRadian(vec.y);
	temp.z = Math::ToRadian(vec.z);

	Rotation(temp);
}

void GameRender::RotationDegree(float x, float y, float z)
{
	RotationDegree(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 GameRender::Rotation()
{
	return rotation;
}

D3DXVECTOR3 GameRender::RotationDegree()
{
	D3DXVECTOR3 temp;

	temp.x = Math::ToDegree(rotation.x);
	temp.y = Math::ToDegree(rotation.y);
	temp.z = Math::ToDegree(rotation.z);

	return temp;
}

D3DXMATRIX GameRender::Transformed()
{
	transformed = rootAxis * world;

	if (linkedWorld != NULL)
		transformed = transformed * (*linkedWorld);
	
	return transformed;
}

void GameRender::MoveToDirectionDelta(float moveSpeed, Move moveDirection, float fixedY)
{
	switch (moveDirection)
	{
	case Move::Direction:
		direction.y = fixedY;
		D3DXVec3Normalize(&direction, &direction);
		position = position - direction * moveSpeed * Time::Delta();
		break;
	case Move::Right:
		right.y = fixedY;
		D3DXVec3Normalize(&right, &right);
		position = position - right * moveSpeed * Time::Delta();
		break;
	case Move::Up:
		D3DXVec3Normalize(&up, &up);
		position = position - up * moveSpeed * Time::Delta();
		break;
	}
	UpdateWorld();
}

void GameRender::RotationYToPointDelta(float rotateSpeed, D3DXVECTOR3 targetPosition)
{
	D3DXVECTOR3 targetDir = targetPosition - position;

	D3DXVec3Normalize(&direction, &direction);
	D3DXVec3Normalize(&targetDir, &targetDir);

	float dot;
	float radian;

	dot = D3DXVec3Dot(&direction, &targetDir);
	dot = Math::Clamp(dot, -1.0f, 1.0f);
	radian = acos(dot);

	float maxRadian = rotateSpeed * Time::Delta();
	if (radian > maxRadian)
		radian = maxRadian;

	D3DXVec3Cross(&right, &direction, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	if (D3DXVec3Dot(&right, &targetDir) > 0)
	{
		rotation.y -= radian;
	}
	else
	{
		rotation.y += radian;
	}
	UpdateWorld();
}

void GameRender::Update()
{
}

void GameRender::Render()
{
}

void GameRender::UpdateWorld()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	//D3DXMatrixRotationYawPitchRoll(&R, rotation.y, rotation.x, rotation.z);
	D3DXQUATERNION q;
	D3DXQuaternionRotationYawPitchRoll(&q, rotation.y, rotation.x, rotation.z);
	D3DXMatrixRotationQuaternion(&R, &q);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	world = S * R * T;

	direction = D3DXVECTOR3(world._31, world._32, world._33);
	up = D3DXVECTOR3(world._21, world._22, world._23);
	right = D3DXVECTOR3(world._11, world._12, world._13);
}
