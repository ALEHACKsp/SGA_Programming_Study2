#include "stdafx.h"
#include "GameRender.h"

GameRender::GameRender()
	: enable(true), visible(true)
	, position(0, 0, 0)
	, direction(0, 0, 1), up(0, 1, 0), right(1, 0, 0)
{
	D3DXMatrixIdentity(&rootAxis);
	D3DXMatrixIdentity(&world);
}

GameRender::~GameRender()
{

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

void GameRender::World(D3DXMATRIX & matrix)
{
	world = matrix;

	position = D3DXVECTOR3(world._41, world._42, world._43);

	direction = D3DXVECTOR3(world._31, world._32, world._33);
	up = D3DXVECTOR3(world._21, world._22, world._23);
	right = D3DXVECTOR3(world._11, world._12, world._13);
}

D3DXMATRIX GameRender::World()
{
	return world;
}

void GameRender::Position(D3DXVECTOR3 vec)
{
	position = vec;

	world._41 = vec.x;
	world._42 = vec.y;
	world._43 = vec.z;
}

D3DXVECTOR3 GameRender::Position()
{
	return position;
}

void GameRender::Direction(D3DXVECTOR3 vec)
{
	direction = vec;

	world._31 = vec.x;
	world._32 = vec.y;
	world._33 = vec.z;
}

D3DXVECTOR3 GameRender::Direction()
{
	return direction;
}

void GameRender::Up(D3DXVECTOR3 & vec)
{
	up = vec;

	world._21 = vec.x;
	world._22 = vec.y;
	world._23 = vec.z;
}

D3DXVECTOR3 GameRender::Up()
{
	return up;
}

void GameRender::Right(D3DXVECTOR3 & vec)
{
	right = vec;

	world._11 = right.x;
	world._12 = right.y;
	world._13 = right.z;
}

D3DXVECTOR3 GameRender::Right()
{
	return right;
}

D3DXMATRIX GameRender::Transformed()
{
	return rootAxis * world;
}

void GameRender::Update()
{
}

void GameRender::Render()
{
}