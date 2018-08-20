#pragma once

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

	void World(D3DXMATRIX& matrix);
	D3DXMATRIX World();

	void Position(D3DXVECTOR3 vec);
	D3DXVECTOR3 Position();

	void Direction(D3DXVECTOR3 vec);
	D3DXVECTOR3 Direction();

	void Up(D3DXVECTOR3& vec);
	D3DXVECTOR3 Up();

	void Right(D3DXVECTOR3& vec);
	D3DXVECTOR3 Right();

	D3DXMATRIX Transformed();

	virtual void Update();
	virtual void Render();

protected:
	bool enable;
	bool visible;

	D3DXMATRIX rootAxis;

private:
	D3DXMATRIX world;

	D3DXVECTOR3 position;
	D3DXVECTOR3 direction;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;
};