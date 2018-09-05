#pragma once

#include "Utilities\Memory.h"

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

	virtual void Update();
	virtual void Render();

	void SetName(string name) { this->name = name; }
	string GetName() { return name; }

private:
	void UpdateWorld();

protected:
	bool enable;
	bool visible;

	D3DXMATRIX rootAxis; // 스케일 역할함

	string name;
private:
	D3DXMATRIX world;

	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;

	D3DXVECTOR3 direction;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;

public:
	void SetIsLive(bool isLive) 
	{ 
		if (isMemory)
		{
			if (isLive == true)
				Memory::Do(this, Memory_Create);
			else
				Memory::Do(this, Memory_Delete);
			isMemory = false;
		}

		this->isLive = isLive; 
	}

	bool GetIsLive() { return isLive; }

	void SetIsMemory(bool isMemory) { this->isMemory = isMemory; }
	bool GetIsMemory() { return isMemory; }

private:
	bool isLive;
	bool isMemory;
};