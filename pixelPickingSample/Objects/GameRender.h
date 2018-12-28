#pragma once

enum class Move
{
	Direction,
	Right,
	Up
};

class GameRender
{
public:
	GameRender();
	virtual ~GameRender();

	void Enable(bool val);
	bool Enable();
	vector<function<void(bool)>> Enabled;

	void Visible(bool val);
	bool Visible();
	vector<function<void(bool)>> Visibled;

	void RootAxis(D3DXMATRIX& matrix);
	void RootAxis(D3DXVECTOR3& rotation);
	D3DXMATRIX RootAxis();

	D3DXMATRIX World();
	void World(D3DXMATRIX& world);
	D3DXMATRIX* WorldLink();

	void Position(D3DXVECTOR3& vec);
	void Position(float x, float y, float z);
	D3DXVECTOR3 Position();

	void Scale(D3DXVECTOR3& vec);
	void Scale(float x, float y, float z);
	D3DXVECTOR3 Scale();

	void Rotation(D3DXVECTOR3& vec);
	void Rotation(float x, float y, float z);
	void RotationDegree(D3DXVECTOR3& vec);
	void RotationDegree(float x, float y, float z);
	D3DXVECTOR3 Rotation();
	D3DXVECTOR3 RotationDegree();

	D3DXVECTOR3 Direction();
	D3DXVECTOR3 Up();
	D3DXVECTOR3 Right();

	D3DXMATRIX Transformed();

	void MoveToDirectionDelta(float moveSpeed, Move moveDirection, float fixedY);
	void RotationYToPointDelta(float rotateSpeed, D3DXVECTOR3 targetPosition);

	void LinkWorld(D3DXMATRIX* transform) { linkedWorld = transform; }

	class Transform* GetTransform() { return transform; }

	virtual void Update();
	virtual void Render();

private:
	void UpdateWorld();

protected:
	bool enable;
	bool visible;

	D3DXMATRIX rootAxis;
	D3DXMATRIX* linkedWorld;
	D3DXVECTOR3 linkPosTrans;

private:
	Transform* transform;

	D3DXMATRIX world;
	D3DXMATRIX transformed;

	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;

	D3DXVECTOR3 direction;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;
};