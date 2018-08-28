#include "stdafx.h"
#include "DebugDrawCube.h"

DebugDrawCube::DebugDrawCube(D3DXVECTOR3 & center, D3DXVECTOR3 & halfSize)
{
	name = "Cube";

	this->center = center;
	this->halfSize = halfSize;

	D3DXVECTOR3 lines[24];

	CreateVertex(lines);

	__super::Init(lines, 12);
}

DebugDrawCube::~DebugDrawCube()
{
}

void DebugDrawCube::Render()
{
	__super::Render();
}

void DebugDrawCube::PostRender()
{
	float cp[] = { center.x, center.y,center.z };
	float s[] = { halfSize.x,halfSize.y,halfSize.z };

	D3DXCOLOR color = vertices[0].Color;
	float c[] = { color.r,color.g,color.b,color.a };
	if (ImGui::ColorEdit4("Color", c))
		SetColor(D3DXCOLOR(c));

	if (ImGui::DragFloat3("Center Pos", cp) ||
		ImGui::DragFloat3("HalfSize", s))
		SetPosition(D3DXVECTOR3(cp), D3DXVECTOR3(s));
}

void DebugDrawCube::SetPosition(D3DXVECTOR3& center, D3DXVECTOR3& halfSize)
{
	this->center = center;
	this->halfSize = halfSize;

	D3DXVECTOR3 lines[24];

	CreateVertex(lines);

	for (int i = 0; i < vertexCount; i++)
	{
		vertices[i].Position = lines[i];
	}

	UpdateBuffer();
}

void DebugDrawCube::CreateVertex(D3DXVECTOR3 * lines)
{
	min = center - halfSize;
	max = center + halfSize;

	D3DXVECTOR3 pos[8];

	// ¾Õ - ÁÂÇÏ ÁÂ»ó ¿ìÇÏ ¿ì»ó
	pos[0] = D3DXVECTOR3(min.x, min.y, min.z);
	pos[1] = D3DXVECTOR3(min.x, max.y, min.z);
	pos[2] = D3DXVECTOR3(max.x, min.y, min.z);
	pos[3] = D3DXVECTOR3(max.x, max.y, min.z);

	// µÚ - ÁÂÇÏ ÁÂ»ó ¿ìÇÏ ¿ì»ó
	pos[4] = D3DXVECTOR3(min.x, min.y, max.z);
	pos[5] = D3DXVECTOR3(min.x, max.y, max.z);
	pos[6] = D3DXVECTOR3(max.x, min.y, max.z);
	pos[7] = D3DXVECTOR3(max.x, max.y, max.z);

	D3DXVECTOR3 temp[] =
	{
		// ¾Õ¸é
		pos[0],pos[1], pos[1],pos[3],
		pos[3],pos[2], pos[2],pos[0],

		// µÞ¸é
		pos[4],pos[5], pos[5],pos[7],
		pos[7],pos[6], pos[6],pos[4],

		// ¿ÞÂÊ ¿·
		pos[0],pos[4], pos[1],pos[5],
		// ¿À¸¥ÂÊ ¿·
		pos[2],pos[6], pos[3],pos[7]
	};

	for (int i = 0; i < 24; i++)
		lines[i] = temp[i];
}
