#pragma once

#define CIRCLEGIZMO_SEGMENTS 36

class GameModel;

class Gizmo
{
public:
	Gizmo();
	~Gizmo();

	void Update();
	void Update(D3DXMATRIX& transform);
	void Render();

	void DrawGizmoDirection(GameModel* model, D3DXCOLOR color = D3DXCOLOR(0, 0, 1, 1));
	void DrawGizmoRight(GameModel* model, D3DXCOLOR color = D3DXCOLOR(1, 0, 0, 1));
	void DrawGizmoUp(GameModel* model, D3DXCOLOR color = D3DXCOLOR(0, 1, 0, 1));

	void DrawGizmoRay(string name, D3DXMATRIX* transform, D3DXVECTOR3& direction, D3DXCOLOR color = D3DXCOLOR(1, 0, 0, 1));
	void DrawGizmoBox(string name, D3DXMATRIX* transform, D3DXVECTOR3& minPos, D3DXVECTOR3& maxPos, D3DXCOLOR color = D3DXCOLOR(1, 1, 1, 1));
	void DrawGizmoSphere(string name, D3DXMATRIX* transform, float& radius, D3DXVECTOR3 center = D3DXVECTOR3(0, 0, 0), D3DXCOLOR color = D3DXCOLOR(1, 1, 1, 1));
	void DrawGizmoCapsule(string name, D3DXMATRIX* transform, D3DXVECTOR3& point1, D3DXVECTOR3& point2, float& radius, D3DXCOLOR color = D3DXCOLOR(1, 1, 1, 1));


	void SetActive(bool bOn) { this->bOn = bOn; }
	bool* SetActive() { return &bOn; }

private:
	typedef pair<string, class LineDraw*> Pair;
	map<string, class LineDraw*>::iterator iter;
	map<string, class LineDraw*> gizmos;
	bool bOn;
};