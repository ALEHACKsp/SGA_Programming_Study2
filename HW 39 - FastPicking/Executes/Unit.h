#pragma once

class Unit
{
public:
	static const float HeightOffset;

public:
	Unit(struct MapTile* mapTile, class PathFinding* pathFinding);
	~Unit();

	void Update();
	void Render();

	void Goto(struct MapTile* tile);

private:
	void MoveUnit(struct MapTile* to);

private:
	Material* material;
	MeshSphere* sphere;

	struct MapTile* mapTile;
	vector<struct MapTile *> path;

	D3DXVECTOR3 lastWP, nextWP; // waypoint
	int activeWP;

	D3DXVECTOR3 position; 
	class PathFinding* pathFinding;

	bool bMoving;
	float movePrc; // move process 웨이포인트 사이에서 진행도
	float time;
	float speed;
};