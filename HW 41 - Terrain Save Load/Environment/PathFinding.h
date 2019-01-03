#pragma once
#include "Utilities\PriorityQueue.h"

struct MapTile;

struct MapEdge
{
	MapTile* Node1;
	MapTile* Node2;

	float Cost; // 타일 간의 이동비용

private:
	MapEdge(MapTile* n1, MapTile* n2, float cost)
		: Node1(n1), Node2(n2), Cost(cost)
	{
	}

	static float CalculateCost(MapTile* n1, MapTile* n2);

public:
	static MapEdge* Create(MapTile* tile, MapTile* neighbor);
};

struct MapTile : public IPriorityNode
{
	int Type; // 이 타입에 0번을 갈 수 없게 하는 방법도 있음 Walkable 말고
	bool Walkable;
	int Set;

	float F;
	float G;

	MapTile* Parent;
	MapEdge* Edges[8];

	Point MapPosition;
	D3DXVECTOR3 WorldPosition;

	float Height() { return WorldPosition.y; }

	static const float MaxSlope;
	static const UINT EdgeCount;

	MapTile()
	{
		for (UINT i = 0; i < EdgeCount; i++)
			Edges[i] = NULL;
	}

	~MapTile()
	{
		for (UINT i = 0; i < EdgeCount; i++)
			SAFE_DELETE(Edges[i]);
	}

	bool operator<(const MapTile& a)
	{
		return this->F < a.F;
	}

	// IPriorityNode을(를) 통해 상속됨
	virtual float Priority() override;
	virtual void Priority(float val) override;
	virtual int InsertionIndex() override;
	virtual void InsertionIndex(int val) override;
	virtual int QueueIndex() override;
	virtual void QueueIndex(int val) override;

private:
	float priority = 0;
	int insertionIndex = 0;
	int queueIndex = 0;
};

class PathFinding
{
public:
	PathFinding(class Terrain* terrain, UINT tileSize = 2);
	~PathFinding();

	MapTile* Tile(UINT x, UINT y);
	class Terrain* GetTerrain();

	void GetPath(OUT vector<MapTile*>& tiles, Point& start, Point& end);

private:
	bool Within(Point& p);

	void ResetTileMap();
	void SetTilePositionAndTypes();
	void CalculateWalkAbility();
	void ConnectNeighboringTiles();
	void CreateTileSets();

private:
	class Terrain* terrain;

	UINT tileSize;

	int widthInTiles;
	int heightInTiles;
	MapTile* tiles;
};
