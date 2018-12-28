#include "stdafx.h"
#include "LineDraw.h"

LineDraw::LineDraw(D3DXMATRIX* transform)
	: transform(transform), pushBuffer(false), count(0), lineType(LINE_END)
	, vertices(NULL), indices(NULL), vertexBuffer(NULL), indexBuffer(NULL)
{
	shader = new Shader(Shaders + L"003_Color.hlsl");
	worldBuffer = new WorldBuffer();
	
	//Create CirclePosition
	{
		float intervalAngle = (Math::PI * 2.0f) / CIRCLE_SEGMENTS;
		for (int i = 0; i <= CIRCLE_SEGMENTS; i++) {
			float angle = i * intervalAngle;
			circlePos[i] = D3DXVECTOR2(cos(angle), sin(angle));
		}
	}
}

LineDraw::~LineDraw()
{
	for (LineData* vec : vecLine)
		SAFE_DELETE(vec);
	
	if (vertices != NULL)
	{
		SAFE_DELETE_ARRAY(vertices);
		SAFE_DELETE_ARRAY(indices);
	}
	
	if (indexBuffer != NULL)
	{
		SAFE_RELEASE(indexBuffer);
		SAFE_RELEASE(vertexBuffer);
	}
	
	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);
}

void LineDraw::Update()
{
	worldBuffer->SetMatrix(*transform);
	
	if (pushBuffer)
	{
		PushBuffer();
	}
}

void LineDraw::Update(D3DXMATRIX& transform)
{
	worldBuffer->SetMatrix(transform * (*this->transform));

	if (pushBuffer)
	{
		PushBuffer();
	}
}

void LineDraw::Render()
{
	if (count > 0)
	{
		UINT stride = sizeof(VertexColor);
		UINT offset = 0;
	
		D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	
		worldBuffer->SetVSBuffer(1);
	
		shader->Render();
	
		D3D::GetDC()->DrawIndexed(count, 0, 0);
	}
}

void LineDraw::PushBuffer()
{
	SAFE_DELETE_ARRAY(vertices);
	SAFE_DELETE_ARRAY(indices);

	vertices = new VertexColor[count];
	indices = new UINT[count];

	for (UINT i = 0; i < vecLine.size(); i++)
	{
		vertices[i * 2 + 0].Position = vecLine[i]->Start;
		vertices[i * 2 + 1].Position = vecLine[i]->End;

		vertices[i * 2 + 0].Color = vecLine[i]->Color;
		vertices[i * 2 + 1].Color = vecLine[i]->Color;

		indices[i * 2 + 0] = i * 2 + 0;
		indices[i * 2 + 1] = i * 2 + 1;
	}
	
	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(VertexColor) * count;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = vertices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr));
	}

	//Create Index Buffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(UINT) * count;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = indices;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(&desc, &data, &indexBuffer);
		assert(SUCCEEDED(hr));
	}
	pushBuffer = false;
}

void LineDraw::AddLine(D3DXVECTOR3& start, D3DXVECTOR3& end, D3DXCOLOR color)
{
	LineData* temp = new LineData();
	temp->Start = start;
	temp->End = end;
	temp->Color = color;

	vecLine.push_back(temp);
	count = vecLine.size() * 2;
	pushBuffer = true;
}

void LineDraw::ChangeLine(LineData * line, D3DXVECTOR3 & start, D3DXVECTOR3 & end, D3DXCOLOR color)
{
	line->Start = start;
	line->End = end;
	line->Color = color;

	pushBuffer = true;
}

void LineDraw::ChangeColor(D3DXCOLOR color)
{
	for (LineData* vec : vecLine)
	{
		vec->Color = color;
	}
	pushBuffer = true;
}

void LineDraw::Clear()
{
	for (LineData* vec : vecLine)
		SAFE_DELETE(vec);
	vecLine.clear();
	count = 0;
}

void LineDraw::DrawLine(D3DXVECTOR3 & start, D3DXVECTOR3 & end, D3DXCOLOR color)
{
	if (lineType == LINE_LINE)
	{
		for (LineData* vec : vecLine)
		{
			ChangeLine(vec, start, end, color);
		}
	}
	else
	{
		Clear();
		AddLine(start, end, color);
		vecLine.assign(vecLine.begin(), vecLine.end());

		lineType = LINE_LINE;
	}
}

void LineDraw::DrawBox(D3DXVECTOR3 & minPos, D3DXVECTOR3 & maxPos, D3DXCOLOR color)
{
	if (lineType == LINE_BOX)
	{
		for (LineData* vec : vecLine)
		{
			ChangeLine(vec, D3DXVECTOR3(minPos.x, minPos.y, minPos.z), D3DXVECTOR3(maxPos.x, minPos.y, minPos.z), color);
			ChangeLine(vec, D3DXVECTOR3(maxPos.x, minPos.y, minPos.z), D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z), color);
			ChangeLine(vec, D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z), D3DXVECTOR3(minPos.x, minPos.y, maxPos.z), color);
			ChangeLine(vec, D3DXVECTOR3(minPos.x, minPos.y, maxPos.z), D3DXVECTOR3(minPos.x, minPos.y, minPos.z), color);
			
			ChangeLine(vec, D3DXVECTOR3(minPos.x, maxPos.y, minPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z), color);
			ChangeLine(vec, D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z), color);
			ChangeLine(vec, D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z), D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z), color);
			ChangeLine(vec, D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z), D3DXVECTOR3(minPos.x, maxPos.y, minPos.z), color);
			
			ChangeLine(vec, D3DXVECTOR3(minPos.x, minPos.y, minPos.z), D3DXVECTOR3(minPos.x, maxPos.y, minPos.z), color);
			ChangeLine(vec, D3DXVECTOR3(maxPos.x, minPos.y, minPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z), color);
			ChangeLine(vec, D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z), color);
			ChangeLine(vec, D3DXVECTOR3(minPos.x, minPos.y, maxPos.z), D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z), color);
		}
	}
	else
	{
		Clear();
		AddLine(D3DXVECTOR3(minPos.x, minPos.y, minPos.z), D3DXVECTOR3(maxPos.x, minPos.y, minPos.z), color);
		AddLine(D3DXVECTOR3(maxPos.x, minPos.y, minPos.z), D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z), color);
		AddLine(D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z), D3DXVECTOR3(minPos.x, minPos.y, maxPos.z), color);
		AddLine(D3DXVECTOR3(minPos.x, minPos.y, maxPos.z), D3DXVECTOR3(minPos.x, minPos.y, minPos.z), color);

		AddLine(D3DXVECTOR3(minPos.x, maxPos.y, minPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z), color);
		AddLine(D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z), color);
		AddLine(D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z), D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z), color);
		AddLine(D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z), D3DXVECTOR3(minPos.x, maxPos.y, minPos.z), color);

		AddLine(D3DXVECTOR3(minPos.x, minPos.y, minPos.z), D3DXVECTOR3(minPos.x, maxPos.y, minPos.z), color);
		AddLine(D3DXVECTOR3(maxPos.x, minPos.y, minPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, minPos.z), color);
		AddLine(D3DXVECTOR3(maxPos.x, minPos.y, maxPos.z), D3DXVECTOR3(maxPos.x, maxPos.y, maxPos.z), color);
		AddLine(D3DXVECTOR3(minPos.x, minPos.y, maxPos.z), D3DXVECTOR3(minPos.x, maxPos.y, maxPos.z), color);
		vecLine.assign(vecLine.begin(), vecLine.end());

		lineType = LINE_BOX;
	}
}

void LineDraw::DrawRect(D3DXVECTOR3 & center, float width, float height, D3DXCOLOR color)
{
	if (lineType == LINE_RECT)
	{
		for (LineData* vec : vecLine)
		{
			ChangeLine(vec, D3DXVECTOR3(center.x - width, center.y - height, center.z), D3DXVECTOR3(center.x + width, center.y - height, center.z), color);
			ChangeLine(vec, D3DXVECTOR3(center.x + width, center.y - height, center.z), D3DXVECTOR3(center.x + width, center.y + height, center.z), color);
			ChangeLine(vec, D3DXVECTOR3(center.x + width, center.y + height, center.z), D3DXVECTOR3(center.x - width, center.y + height, center.z), color);
			ChangeLine(vec, D3DXVECTOR3(center.x - width, center.y + height, center.z), D3DXVECTOR3(center.x - width, center.y - height, center.z), color);
		}
	}
	else
	{
		Clear();
		AddLine(D3DXVECTOR3(center.x - width, center.y - height, center.z), D3DXVECTOR3(center.x + width, center.y - height, center.z), color);
		AddLine(D3DXVECTOR3(center.x + width, center.y - height, center.z), D3DXVECTOR3(center.x + width, center.y + height, center.z), color);
		AddLine(D3DXVECTOR3(center.x + width, center.y + height, center.z), D3DXVECTOR3(center.x - width, center.y + height, center.z), color);
		AddLine(D3DXVECTOR3(center.x - width, center.y + height, center.z), D3DXVECTOR3(center.x - width, center.y - height, center.z), color);
		vecLine.assign(vecLine.begin(), vecLine.end());

		lineType = LINE_RECT;
	}
}

void LineDraw::DrawSphere(D3DXVECTOR3& center, float radius, D3DXCOLOR color)
{
	if (lineType == LINE_SPHERE)
	{
		UINT index = 0;
		float deltaTheta = 3.14159265f / CIRCLE_SEGMENTS;
		float deltaPhi = 2.0f * 3.14159265f / CIRCLE_SEGMENTS;
		float theta = -deltaTheta;
		float phi = 0;
		//¸î¹øÂ° ¸µÀ» ±×¸®´Â°¡
		for (UINT ring = 0; ring <= CIRCLE_SEGMENTS; ring++)
		{
			theta += deltaTheta;
			float sinTheta = sin(theta);
			float cosTheta = cos(theta);
			for (UINT point = 0; point <= CIRCLE_SEGMENTS; point++)
			{
				if (phi > 2.0f * 3.14159265f)
					phi = 0.0f;
				phi += deltaPhi;
				UINT index = (CIRCLE_SEGMENTS + 1) * ring + point;
				D3DXVECTOR3 position[2];
				position[0].x = center.z + sinTheta * sin(phi) * radius;
				position[0].y = center.y + cosTheta * radius;
				position[0].z = center.x + sinTheta * cos(phi) * radius;

				position[1].x = center.z + sinTheta * sin(phi + deltaPhi) * radius;
				position[1].y = center.y + cosTheta * radius;
				position[1].z = center.x + sinTheta * cos(phi + deltaPhi) * radius;

				ChangeLine(vecLine[index++], position[0], position[1], color);
			}//for(point)
		}//for(ring)
	}
	else
	{
		Clear();
		float deltaTheta = 3.14159265f / CIRCLE_SEGMENTS;
		float deltaPhi = 2.0f * 3.14159265f / CIRCLE_SEGMENTS;
		float theta = -deltaTheta;
		float phi = 0;
		//¸î¹øÂ° ¸µÀ» ±×¸®´Â°¡
		for (UINT ring = 0; ring <= CIRCLE_SEGMENTS; ring++)
		{
			theta += deltaTheta;
			float sinTheta = sin(theta);
			float cosTheta = cos(theta);
			for (UINT point = 0; point <= CIRCLE_SEGMENTS; point++)
			{
				if (phi > 2.0f * 3.14159265f)
					phi = 0.0f;
				phi += deltaPhi;
				UINT index = (CIRCLE_SEGMENTS + 1) * ring + point;
				D3DXVECTOR3 position[2];
				position[0].x = center.z + sinTheta * sin(phi) * radius;
				position[0].y = center.y + cosTheta * radius;
				position[0].z = center.x + sinTheta * cos(phi) * radius;

				position[1].x = center.z + sinTheta * sin(phi + deltaPhi) * radius;
				position[1].y = center.y + cosTheta * radius;
				position[1].z = center.x + sinTheta * cos(phi + deltaPhi) * radius;

				AddLine(position[0], position[1], color);
			}//for(point)
		}//for(ring)
		vecLine.assign(vecLine.begin(), vecLine.end());

		lineType = LINE_SPHERE;
	}	
}

void LineDraw::DrawCapsule(D3DXVECTOR3 & point1, D3DXVECTOR3 & point2, float radius, D3DXCOLOR color)
{
	if (lineType == LINE_CAPSULE)
	{
		UINT index = 0;

		int harfCircle = CIRCLE_SEGMENTS / 2;
		int quarterCircle = CIRCLE_SEGMENTS / 4;
		for (int i = 0; i < CIRCLE_SEGMENTS; i++)
		{
			//À­ºÎºÐ
			{
				//¶Ñ²±
				if (i < quarterCircle || i >= quarterCircle * 3)
					ChangeLine(vecLine[index++], point1 + D3DXVECTOR3(0.0f, circlePos[i].x, circlePos[i].y) * radius, point1 + D3DXVECTOR3(0.0f, circlePos[i + 1].x, circlePos[i + 1].y) * radius, color);
				if (i < harfCircle)
					ChangeLine(vecLine[index++], point1 + D3DXVECTOR3(circlePos[i].x, circlePos[i].y, 0.0f) * radius, point1 + D3DXVECTOR3(circlePos[i + 1].x, circlePos[i + 1].y, 0.0f) * radius, color);
				//¸µ
				ChangeLine(vecLine[index++], point1 + D3DXVECTOR3(circlePos[i].x, 0.0f, circlePos[i].y) * radius, point1 + D3DXVECTOR3(circlePos[i + 1].x, 0.0f, circlePos[i + 1].y) * radius, color);
			}

			//¾Æ·§ºÎºÐ
			{
				//¶Ñ²±
				if (i >= quarterCircle && i < quarterCircle * 3)
					ChangeLine(vecLine[index++], point2 + D3DXVECTOR3(0.0f, circlePos[i].x, circlePos[i].y) * radius, point2 + D3DXVECTOR3(0.0f, circlePos[i + 1].x, circlePos[i + 1].y) * radius, color);
				if (i >= harfCircle)
					ChangeLine(vecLine[index++], point2 + D3DXVECTOR3(circlePos[i].x, circlePos[i].y, 0.0f) * radius, point2 + D3DXVECTOR3(circlePos[i + 1].x, circlePos[i + 1].y, 0.0f) * radius, color);
				//¸µ
				ChangeLine(vecLine[index++], point2 + D3DXVECTOR3(circlePos[i].x, 0.0f, circlePos[i].y) * radius, point2 + D3DXVECTOR3(circlePos[i + 1].x, 0.0f, circlePos[i + 1].y) * radius, color);
			}
		}
		//Áß°£ºÎºÐ
		ChangeLine(vecLine[index++], point1 + D3DXVECTOR3(circlePos[0].x, 0.0f, circlePos[0].y) * radius, point2 + D3DXVECTOR3(circlePos[0].x, 0.0f, circlePos[0].y) * radius, color);
		ChangeLine(vecLine[index++], point1 + D3DXVECTOR3(circlePos[quarterCircle].x, 0.0f, circlePos[quarterCircle].y) * radius, point2 + D3DXVECTOR3(circlePos[quarterCircle].x, 0.0f, circlePos[quarterCircle].y) * radius, color);
		ChangeLine(vecLine[index++], point1 + D3DXVECTOR3(circlePos[harfCircle].x, 0.0f, circlePos[harfCircle].y) * radius, point2 + D3DXVECTOR3(circlePos[harfCircle].x, 0.0f, circlePos[harfCircle].y) * radius, color);
		ChangeLine(vecLine[index++], point1 + D3DXVECTOR3(circlePos[harfCircle + quarterCircle].x, 0.0f, circlePos[harfCircle + quarterCircle].y) * radius, point2 + D3DXVECTOR3(circlePos[harfCircle + quarterCircle].x, 0.0f, circlePos[harfCircle + quarterCircle].y) * radius, color);
	}
	else
	{
		Clear();
		int harfCircle = CIRCLE_SEGMENTS / 2;
		int quarterCircle = CIRCLE_SEGMENTS / 4;
		for (int i = 0; i < CIRCLE_SEGMENTS; i++)
		{
			//À­ºÎºÐ
			{
				//¶Ñ²±
				if (i < quarterCircle || i >= quarterCircle * 3)
					AddLine(point1 + D3DXVECTOR3(0.0f, circlePos[i].x, circlePos[i].y) * radius, point1 + D3DXVECTOR3(0.0f, circlePos[i + 1].x, circlePos[i + 1].y) * radius, color);
				if (i < harfCircle)
					AddLine(point1 + D3DXVECTOR3(circlePos[i].x, circlePos[i].y, 0.0f) * radius, point1 + D3DXVECTOR3(circlePos[i + 1].x, circlePos[i + 1].y, 0.0f) * radius, color);
				//¸µ
				AddLine(point1 + D3DXVECTOR3(circlePos[i].x, 0.0f, circlePos[i].y) * radius, point1 + D3DXVECTOR3(circlePos[i + 1].x, 0.0f, circlePos[i + 1].y) * radius, color);
			}

			//¾Æ·§ºÎºÐ
			{
				//¶Ñ²±
				if (i >= quarterCircle && i < quarterCircle * 3)
					AddLine(point2 + D3DXVECTOR3(0.0f, circlePos[i].x, circlePos[i].y) * radius, point2 + D3DXVECTOR3(0.0f, circlePos[i + 1].x, circlePos[i + 1].y) * radius, color);
				if (i >= harfCircle)
					AddLine(point2 + D3DXVECTOR3(circlePos[i].x, circlePos[i].y, 0.0f) * radius, point2 + D3DXVECTOR3(circlePos[i + 1].x, circlePos[i + 1].y, 0.0f) * radius, color);
				//¸µ
				AddLine(point2 + D3DXVECTOR3(circlePos[i].x, 0.0f, circlePos[i].y) * radius, point2 + D3DXVECTOR3(circlePos[i + 1].x, 0.0f, circlePos[i + 1].y) * radius, color);
			}
		}
		//Áß°£ºÎºÐ
		AddLine(point1 + D3DXVECTOR3(circlePos[0].x, 0.0f, circlePos[0].y) * radius, point2 + D3DXVECTOR3(circlePos[0].x, 0.0f, circlePos[0].y) * radius, color);
		AddLine(point1 + D3DXVECTOR3(circlePos[quarterCircle].x, 0.0f, circlePos[quarterCircle].y) * radius, point2 + D3DXVECTOR3(circlePos[quarterCircle].x, 0.0f, circlePos[quarterCircle].y) * radius, color);
		AddLine(point1 + D3DXVECTOR3(circlePos[harfCircle].x, 0.0f, circlePos[harfCircle].y) * radius, point2 + D3DXVECTOR3(circlePos[harfCircle].x, 0.0f, circlePos[harfCircle].y) * radius, color);
		AddLine(point1 + D3DXVECTOR3(circlePos[harfCircle + quarterCircle].x, 0.0f, circlePos[harfCircle + quarterCircle].y) * radius, point2 + D3DXVECTOR3(circlePos[harfCircle + quarterCircle].x, 0.0f, circlePos[harfCircle + quarterCircle].y) * radius, color);
		vecLine.assign(vecLine.begin(), vecLine.end());

		lineType = LINE_CAPSULE;
	}
}

void LineDraw::DrawCircle(D3DXVECTOR3 & center, float radius, D3DXCOLOR color)
{
	if (lineType == LINE_CIRCLE)
	{
		UINT index = 0;
		
		for (int i = 0; i < CIRCLE_SEGMENTS; i++)
		{
			ChangeLine(vecLine[index++], center + D3DXVECTOR3(circlePos[i].x, circlePos[i].y, 0.0f) * radius, center + D3DXVECTOR3(circlePos[i + 1].x, circlePos[i + 1].y, 0.0f) * radius, color);
			ChangeLine(vecLine[index++], center + D3DXVECTOR3(circlePos[i].x, 0.0f, circlePos[i].y) * radius, center + D3DXVECTOR3(circlePos[i + 1].x, 0.0f, circlePos[i + 1].y) * radius, color);
			ChangeLine(vecLine[index++], center + D3DXVECTOR3(0.0f, circlePos[i].x, circlePos[i].y) * radius, center + D3DXVECTOR3(0.0f, circlePos[i + 1].x, circlePos[i + 1].y) * radius, color);
		}
	}
	else
	{
		Clear();
		for (int i = 0; i < CIRCLE_SEGMENTS; i++)
		{
			AddLine(center + D3DXVECTOR3(circlePos[i].x, circlePos[i].y, 0.0f) * radius, center + D3DXVECTOR3(circlePos[i + 1].x, circlePos[i + 1].y, 0.0f) * radius, color);
			AddLine(center + D3DXVECTOR3(circlePos[i].x, 0.0f, circlePos[i].y) * radius, center + D3DXVECTOR3(circlePos[i + 1].x, 0.0f, circlePos[i + 1].y) * radius, color);
			AddLine(center + D3DXVECTOR3(0.0f, circlePos[i].x, circlePos[i].y) * radius, center + D3DXVECTOR3(0.0f, circlePos[i + 1].x, circlePos[i + 1].y) * radius, color);
		}
		vecLine.assign(vecLine.begin(), vecLine.end());

		lineType = LINE_CIRCLE;
	}
}

void LineDraw::DrawCone(D3DXVECTOR3 & center, float range, float angle, D3DXCOLOR color)
{
	//angleÀº direction°úÀÇ °¢µµ
	if (lineType == LINE_CONE)
	{
		UINT index = 0;
		//circle
		float radius = range / cosf(angle) * sinf(angle);
		for (int i = 0; i < CIRCLE_SEGMENTS; i++)
		{
			ChangeLine(vecLine[index++], center + D3DXVECTOR3(circlePos[i].x * radius, circlePos[i].y * radius, range), center + D3DXVECTOR3(circlePos[i + 1].x * radius, circlePos[i + 1].y * radius, range), color);
		}

		//cone line
		for (int i = 0; i < 4; i++)
			ChangeLine(vecLine[index++], center, center + D3DXVECTOR3(circlePos[CIRCLE_SEGMENTS / 4 * i].x * radius, circlePos[CIRCLE_SEGMENTS / 4 * i].y * radius, range), color);
	}
	else
	{
		Clear();

		float radius = range / cosf(angle) * sinf(angle);
		for (int i = 0; i < CIRCLE_SEGMENTS; i++)
		{
			AddLine(center + D3DXVECTOR3(circlePos[i].x * radius, circlePos[i].y * radius, range), center + D3DXVECTOR3(circlePos[i + 1].x * radius, circlePos[i + 1].y * radius, range), color);
		}

		//cone line
		for (int i = 0; i < 4; i++)
			AddLine(center, center + D3DXVECTOR3(circlePos[CIRCLE_SEGMENTS / 4 * i].x * radius, circlePos[CIRCLE_SEGMENTS / 4 * i].y * radius, range), color);
	}
	vecLine.assign(vecLine.begin(), vecLine.end());

	lineType = LINE_CONE;
}
