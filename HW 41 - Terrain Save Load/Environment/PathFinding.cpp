#include "stdafx.h"
#include "PathFinding.h"

#include "Terrain.h"
#include "HeightMap.h"

const float MapTile::MaxSlope = 0.6f;
const UINT MapTile::EdgeCount = 8;

PathFinding::PathFinding(Terrain* terrain, UINT tileSize)
	: terrain(terrain), tileSize(tileSize)
{
	tiles = NULL;

	ResetTileMap();
	SetTilePositionAndTypes();
	CalculateWalkAbility();
	ConnectNeighboringTiles();
	CreateTileSets();
}

PathFinding::~PathFinding()
{
	SAFE_DELETE_ARRAY(tiles);
}

MapTile * PathFinding::Tile(UINT x, UINT y) 
{ 
	if (x < 0 || y < 0 || x >= widthInTiles || y >= heightInTiles) return NULL;

	return &tiles[x + y * heightInTiles]; 
}

Terrain * PathFinding::GetTerrain()
{
	return terrain;
}

void PathFinding::GetPath(OUT vector<MapTile*>& tiles, Point & start, Point & end)
{
	MapTile* startTile = Tile(start.X, start.Y);
	MapTile* endTile = Tile(end.X, end.Y);

	bool b = false;
	b |= Within(start) == false;
	b |= Within(end) == false;
	b |= start == end;
	b |= startTile == NULL;
	b |= endTile == NULL;

	if (b == true) return;

	for (int i = 0; i < widthInTiles * heightInTiles; i++)
		this->tiles[i].F = this->tiles[i].G = FLT_MAX;

	PriorityQueue open(widthInTiles * heightInTiles);
	unordered_set<MapTile*> closed;

	startTile->G = 0;

	float h = Math::DiagonalDistance2(start, end);

	startTile->F = h;

	open.Enqueue(startTile, startTile->F);
	MapTile* current = NULL;
	while (open.Empty() == false && current != endTile)
	{
		current = (MapTile*)open.Dequeue();

		closed.insert(current);
		for (int i = 0; i < MapTile::EdgeCount; i++)
		{
			MapEdge* edge = current->Edges[i];
			if (edge == NULL) continue;

			MapTile* neighbor = edge->Node2;
			float cost = current->G + edge->Cost;

			if (open.Contain(neighbor) && cost < neighbor->G)
				open.Remove(neighbor);

			unordered_set<MapTile*>::iterator iter = closed.find(neighbor);
			if (iter != closed.end() && cost < neighbor->G)
				closed.erase(iter);

			iter = closed.find(neighbor);
			if (open.Contain(neighbor) == false && iter == closed.end())
			{
				neighbor->G = cost;
				float f = cost + Math::DiagonalDistance2(neighbor->MapPosition, end);

				open.Enqueue(neighbor, f);
				neighbor->Parent = current;
			} // if(open)
		} // for (i)
	} // while

	while (current != startTile)
	{
		tiles.push_back(current);
		current = current->Parent;
	}

	reverse(tiles.begin(), tiles.end());
}

bool PathFinding::Within(Point & p)
{
	return p.X >= 0 && p.X < widthInTiles && p.Y >= 0 && p.Y < heightInTiles;
}

void PathFinding::ResetTileMap()
{
	widthInTiles = (int)terrain->Desc().HeightMapWidth / tileSize;
	heightInTiles = (int)terrain->Desc().HeightMapHeight / tileSize;

	tiles = new MapTile[widthInTiles * heightInTiles];
}

void PathFinding::SetTilePositionAndTypes()
{
	Terrain::InitDesc desc = terrain->Desc();

	float maxHeight = terrain->GetHeightMap()->MaxHeight();

	for (int y = 0; y < heightInTiles; y++)
	{
		for (int x = 0; x < widthInTiles; x++)
		{
			MapTile* tile = Tile(x, y);
			tile->MapPosition = Point(x, y);

			float worldX = 
				(x * desc.CellSpacing * tileSize) 
				+ (desc.CellSpacing * tileSize / 2) - (terrain->Width() / 2);
			float worldZ =
				(-y * desc.CellSpacing * tileSize)
				- (desc.CellSpacing * tileSize / 2) + (terrain->Depth() / 2);
			float height = terrain->GetHeight(worldX, worldZ);
			tile->WorldPosition = D3DXVECTOR3(worldX, height, worldZ);

			// Set tile type 
			// 지형 툴에서는 선택해서 type 설정할 수 있게 해줘야함
			if (tile->Height() < maxHeight * (0.05f)) tile->Type = 0;
			else if (tile->Height() < maxHeight * (0.4f)) tile->Type = 1;
			else if (tile->Height() < maxHeight * (0.75f)) tile->Type = 2;
			else tile->Type = 3;
		} // for(y)
	} // for(x)
}

void PathFinding::CalculateWalkAbility()
{
	for (int y = 0; y < heightInTiles; y++)
	{
		for (int x = 0; x < widthInTiles; x++)
		{
			MapTile* tile = Tile(x, y);
			
			if (tile == NULL) continue;
			
			Point points[] = {
				Point(x - 1, y - 1), Point(x, y - 1), Point(x + 1, y - 1),
				Point(x - 1, y), Point(x + 1, y),
				Point(x - 1, y + 1), Point(x, y + 1), Point(x + 1, y + 1)
			};

			float variance = 0.0f;
			int n = 0;

			for (int i = 0; i < 8; i++)
			{
				if (!Within(points[i])) continue;

				MapTile* neighbor = Tile(points[i].X, points[i].Y);
				if (neighbor == NULL) continue;

				float v = neighbor->Height() - tile->Height();

				if (v <= 0.01f) continue;

				variance += v * v;
				n++;
			}

			if (n == 0) n = 1;
			variance /= n;

			tile->Walkable = variance < MapTile::MaxSlope;
		}
	}
}

void PathFinding::ConnectNeighboringTiles()
{
	for (int y = 0; y < heightInTiles; y++)
	{
		for (int x = 0; x < widthInTiles; x++)
		{
			MapTile* tile = Tile(x, y);

			if (tile == NULL || !tile->Walkable) continue;

			for (int i = 0; i < MapTile::EdgeCount; i++)
				tile->Edges[i] = NULL;

			Point points[] = {
				Point(x - 1, y - 1), Point(x, y - 1), Point(x + 1, y - 1),
				Point(x - 1, y), Point(x + 1, y),
				Point(x - 1, y + 1), Point(x, y + 1), Point(x + 1, y + 1)
			};

			for (int i = 0; i < MapTile::EdgeCount; i++) {
				if (!Within(points[i])) continue;

				MapTile* neighbor = Tile(points[i].X, points[i].Y);
				if (neighbor != NULL && neighbor->Walkable)
					tile->Edges[i] = MapEdge::Create(tile, neighbor);
			}
		}
	}
}

void PathFinding::CreateTileSets()
{
	int setNo = 0;
	unordered_set<MapTile*> unVisited;

	for (int y = 0; y < heightInTiles; y++) 
	{
		for (int x = 0; x < widthInTiles; x++) 
		{
			MapTile* tile = Tile(x, y);

			for (int i = 0; i < MapTile::EdgeCount; i++)
			{
				if (tile->Edges[i] != NULL)
				{
					if (tile->Walkable)
						unVisited.insert(tile);
					else
						tile->Set = --setNo;
				}
				else
				{
					tile->Set = --setNo;
				}
			} // for(i)
		} // for(x)
	} // for(y)

	setNo = 0;
	stack<MapTile*> stk;

	while (unVisited.empty() == false)
	{
		unordered_set<MapTile*>::iterator newFirst = unVisited.begin();
		stk.push(*newFirst);
		unVisited.erase(newFirst);

		while (stk.empty() == false)
		{
			MapTile* next = stk.top();
			stk.pop();

			next->Set = setNo;

			for (int i = 0; i < MapTile::EdgeCount; i++)
			{
				if (next->Edges[i] != NULL 
					&& unVisited.find(next->Edges[i]->Node2) != unVisited.end())
				{
					stk.push(next->Edges[i]->Node2);
					unVisited.erase(next->Edges[i]->Node2);
				}
			} // for(i)
		} // stack

		setNo++;
	} // set
}

float MapEdge::CalculateCost(MapTile * n1, MapTile * n2)
{
	float dx = fabsf(n1->WorldPosition.x - n2->WorldPosition.x);
	float dz = fabsf(n1->WorldPosition.z - n2->WorldPosition.z);
	float dy = fabsf(n1->WorldPosition.y - n2->WorldPosition.y);

	float dxz = sqrtf(dx * dx + dz * dz);
	float slope = dy / dxz;

	return 1 + slope;
}

MapEdge * MapEdge::Create(MapTile * tile, MapTile * neighbor)
{
	float cost = CalculateCost(tile, neighbor);
	if (cost < 1 + MapTile::MaxSlope)
		return new MapEdge(tile, neighbor, cost);

	return NULL;
}

float MapTile::Priority()
{
	return priority;
}

void MapTile::Priority(float val)
{
	priority = val;
}

int MapTile::InsertionIndex()
{
	return insertionIndex;
}

void MapTile::InsertionIndex(int val)
{
	insertionIndex = val;
}

int MapTile::QueueIndex()
{
	return queueIndex;
}

void MapTile::QueueIndex(int val)
{
	queueIndex = val;
}
