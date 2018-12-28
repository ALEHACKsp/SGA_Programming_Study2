#pragma once

#define CIRCLE_SEGMENTS 36

class LineDraw
{
private:
	struct LineData
	{
		D3DXVECTOR3 Start;
		D3DXVECTOR3 End;
		D3DXCOLOR Color;
	};
	enum LineType
	{
		LINE_LINE,
		LINE_BOX,
		LINE_RECT,
		LINE_SPHERE,
		LINE_CAPSULE,
		LINE_CIRCLE,
		LINE_CONE,
		LINE_END
	};
public:
	LineDraw(D3DXMATRIX* transform);
	~LineDraw();
	
	void Update();
	void Update(D3DXMATRIX& transform);
	void Render();

	void PushBuffer();
	void AddLine(D3DXVECTOR3& start, D3DXVECTOR3& end, D3DXCOLOR color = D3DXCOLOR(0, 0, 0, 1));
	void ChangeLine(LineData* line, D3DXVECTOR3& start, D3DXVECTOR3& end, D3DXCOLOR color = D3DXCOLOR(0, 0, 0, 1));
	void ChangeColor(D3DXCOLOR color);

	void Clear();

	void DrawLine(D3DXVECTOR3& start, D3DXVECTOR3& end, D3DXCOLOR color = D3DXCOLOR(0, 0, 0, 1));
	void DrawBox(D3DXVECTOR3& minPos, D3DXVECTOR3& maxPos, D3DXCOLOR color = D3DXCOLOR(1, 1, 1, 1));
	void DrawRect(D3DXVECTOR3 & center, float width, float height, D3DXCOLOR color = D3DXCOLOR(1, 1, 1, 1));
	void DrawSphere(D3DXVECTOR3& center, float radius, D3DXCOLOR color = D3DXCOLOR(1, 1, 1, 1));
	void DrawCapsule(D3DXVECTOR3& point1, D3DXVECTOR3& point2, float radius, D3DXCOLOR color = D3DXCOLOR(1, 1, 1, 1));
	void DrawCircle(D3DXVECTOR3& center, float radius, D3DXCOLOR color = D3DXCOLOR(1, 1, 1, 1));
	void DrawCone(D3DXVECTOR3& center, float range, float angle, D3DXCOLOR color = D3DXCOLOR(1, 1, 1, 1));

private:	
	vector<LineData *> vecLine;
	vector<LineData *>::iterator iter;

	D3DXMATRIX* transform;

	UINT count;

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	
	VertexColor* vertices;
	UINT* indices;

	Shader* shader;
	WorldBuffer* worldBuffer;

	bool pushBuffer;

	LineType lineType;
	D3DXVECTOR2 circlePos[CIRCLE_SEGMENTS + 1];
};