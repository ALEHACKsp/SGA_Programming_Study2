#include "stdafx.h"
#include "Line.h"

Line::Line()
	: vertices(NULL)
	, position(0, 0, 0), scale(1, 1, 1), rotation(0, 0, 0)
	, direction(0, 0, 1), up(0, 1, 0), right(1, 0, 0)
{
	D3DXMatrixIdentity(&world);
}

Line::~Line()
{
	// dx로부터 만들어진건 release
	// 보통 역순으로 릴리즈함
	//SAFE_RELEASE(indexBuffer);
	SAFE_RELEASE(vertexBuffer);

	//SAFE_DELETE_ARRAY(indices);
	SAFE_DELETE_ARRAY(vertices);

	SAFE_DELETE(colorBuffer);

	SAFE_DELETE(worldBuffer);
	SAFE_DELETE(shader);
}

void Line::Init(D3DXVECTOR3 * positions, UINT lineCount, D3DXCOLOR color, 
	D3D11_PRIMITIVE_TOPOLOGY topology)
{
	shader = new Shader(Shaders + L"003_Color.hlsl");
	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;

	this->topology = topology;
	
	// Create VertexData
	{
		if (topology == D3D11_PRIMITIVE_TOPOLOGY_LINELIST)
			vertexCount = lineCount * 2;
		else if (topology == D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP)
			vertexCount = lineCount + 1;

		vertices = new VertexType[vertexCount];

		for (UINT i = 0; i < vertexCount; i++) {
			vertices[i].Position = positions[i];
			vertices[i].Color = color;
		}
	}

	// CreateVertexBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(VertexType) * vertexCount; // 정점 버퍼에 들어갈 데이터의 크기
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
		data.pSysMem = vertices; // 쓸 데이터의 주소

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &vertexBuffer);
		assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	}
}

void Line::Render()
{
	UINT stride = sizeof(VertexType); // 그릴 크기
	UINT offset = 0;

	// vertex buffer 여러개 들어갈 수 있음
	// IA 붙는 이유 나중에 설명해주실꺼
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	//D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// 그릴 방식 설정
	D3D::GetDC()->IASetPrimitiveTopology(topology);

	colorBuffer->SetPSBuffer(0); // 레지스터 번호
	worldBuffer->SetMatrix(World());
	worldBuffer->SetVSBuffer(1);
	shader->Render();

	// 실제로 그리는 거
	//D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
	D3D::GetDC()->Draw(vertexCount, 0);
}

void Line::PostRender()
{
	ImGui::Text("Transform");

	D3DXVECTOR3 S, R, T;
	S = Scale();
	R = Rotation();
	T = Position();

	ImGui::Text("Position");
	ImGui::SameLine(80);
	ImGui::DragFloat3(" T", (float*)T, 0.1f);

	ImGui::Text("Rotation");
	ImGui::SameLine(80);
	ImGui::DragFloat3(" R", (float*)R, 0.01f);

	ImGui::Text("Scale");
	ImGui::SameLine(80);
	ImGui::DragFloat3(" S", (float*)S, 0.01f);

	Scale(S);
	Rotation(R);
	Position(T);
}

void Line::SetColor(D3DXCOLOR color)
{
	D3DXCOLOR prevColor = vertices[0].Color;
	if (color == prevColor)
		return;

	for (UINT i = 0; i < vertexCount; i++) {
		vertices[i].Color = color;
	}

	UpdateBuffer();
}

void Line::UpdateBuffer()
{
	D3D::GetDC()->UpdateSubresource(
		vertexBuffer, 0, NULL, &vertices[0],
		sizeof(VertexType) * vertexCount, 0
	);
}

void Line::SetLine(D3DXVECTOR3 * positions, UINT lineCount, D3D11_PRIMITIVE_TOPOLOGY topology, bool isChange)
{
	D3DXCOLOR color = vertices[0].Color;
	SAFE_DELETE(vertices);

	// Create VertexData
	{
		this->topology = topology;

		if (topology == D3D11_PRIMITIVE_TOPOLOGY_LINELIST)
			vertexCount = lineCount * 2;
		else if (topology == D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP)
			vertexCount = lineCount + 1;

		vertices = new VertexType[vertexCount];

		for (UINT i = 0; i < vertexCount; i++) {
			vertices[i].Position = positions[i];
			vertices[i].Color = color;
		}
	}

	if(isChange == false)
		UpdateBuffer();
	else {
		SAFE_RELEASE(vertexBuffer);
		// CreateVertexBuffer
		{
			D3D11_BUFFER_DESC desc = { 0 };
			desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
			desc.ByteWidth = sizeof(VertexType) * vertexCount; // 정점 버퍼에 들어갈 데이터의 크기
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

			D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
			data.pSysMem = vertices; // 쓸 데이터의 주소

			HRESULT hr = D3D::GetDevice()->CreateBuffer(
				&desc, &data, &vertexBuffer);
			assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
		}
	}
}

D3DXMATRIX Line::World()
{
	return world;
}

void Line::Position(D3DXVECTOR3 & vec)
{
	position = vec;

	UpdateWorld();
}

void Line::Position(float x, float y, float z)
{
	Position(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 Line::Position()
{
	return position;
}

void Line::Scale(D3DXVECTOR3 & vec)
{
	scale = vec;

	UpdateWorld();
}

void Line::Scale(float x, float y, float z)
{
	Scale(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 Line::Scale()
{
	return scale;
}

void Line::Rotation(D3DXVECTOR3 & vec)
{
	rotation = vec;

	UpdateWorld();
}

void Line::Rotation(float x, float y, float z)
{
	Rotation(D3DXVECTOR3(x, y, z));
}

void Line::RotationDegree(D3DXVECTOR3 & vec)
{
	D3DXVECTOR3 temp;

	temp.x = Math::ToRadian(vec.x);
	temp.y = Math::ToRadian(vec.y);
	temp.z = Math::ToRadian(vec.z);

	Rotation(temp);
}

void Line::RotationDegree(float x, float y, float z)
{
	RotationDegree(D3DXVECTOR3(x, y, z));
}

D3DXVECTOR3 Line::Rotation()
{
	return rotation;
}

D3DXVECTOR3 Line::RotationDegree()
{
	D3DXVECTOR3 temp;

	temp.x = Math::ToDegree(rotation.x);
	temp.y = Math::ToDegree(rotation.y);
	temp.z = Math::ToDegree(rotation.z);

	return temp;
}

D3DXVECTOR3 Line::Direction()
{
	return direction;
}

D3DXVECTOR3 Line::Up()
{
	return up;
}

D3DXVECTOR3 Line::Right()
{
	return right;
}

void Line::UpdateWorld()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(
		&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	world = S * R * T;

	direction = D3DXVECTOR3(world._31, world._32, world._33);
	up = D3DXVECTOR3(world._21, world._22, world._23);
	right = D3DXVECTOR3(world._11, world._12, world._13);
}
