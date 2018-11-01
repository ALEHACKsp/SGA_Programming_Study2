#include "stdafx.h"
#include "Line.h"

Line::Line()
	: vertices(NULL)
	, position(0, 0, 0), scale(1, 1, 1), rotation(0, 0, 0)
	, direction(0, 0, 1), up(0, 1, 0), right(1, 0, 0)
	//, positionInstances(NULL), colorInstances(NULL)
	, colorInstances(NULL)
	, worldInstances(NULL)
{
	D3DXMatrixIdentity(&world);

	bInstance = false;
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
	
	SAFE_DELETE(instanceShader);
	if (bInstance) {
		SAFE_RELEASE(instanceBuffer[0]);
		SAFE_RELEASE(instanceBuffer[1]);
		//SAFE_DELETE_ARRAY(positionInstances);
		SAFE_DELETE_ARRAY(colorInstances);
		//SAFE_RELEASE(instanceBuffer);
		SAFE_DELETE_ARRAY(worldInstances);
	}
}

void Line::Init(D3DXVECTOR3 * positions, UINT lineCount, D3DXCOLOR color, 
	D3D11_PRIMITIVE_TOPOLOGY topology)
{
	shader = new Shader(Shaders + L"Homework/Debug.hlsl");
	instanceShader = new Shader(Shaders + L"Homework/Instance.hlsl", "VS", "PS", true);

	worldBuffer = new WorldBuffer();

	colorBuffer = new ColorBuffer;
	colorBuffer->Data.Color = color;

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
	if (bInstance == false) {
		UINT stride = sizeof(VertexType); // 그릴 크기
		UINT offset = 0;

		// vertex buffer 여러개 들어갈 수 있음
		// IA 붙는 이유 나중에 설명해주실꺼
		D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		//D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		// 그릴 방식 설정
		D3D::GetDC()->IASetPrimitiveTopology(topology);

		colorBuffer->SetPSBuffer(1); // 레지스터 번호
		worldBuffer->SetMatrix(World());
		worldBuffer->SetVSBuffer(1);
		shader->Render();

		// 실제로 그리는 거
		//D3D::GetDC()->DrawIndexed(indexCount, 0, 0);
		D3D::GetDC()->Draw(vertexCount, 0);
	}
	else {
		UINT strides[3];
		UINT offsets[3];
		ID3D11Buffer* buffers[3];

		strides[0] = sizeof(VertexType);
		strides[1] = sizeof(InstanceWorldType);
		//strides[1] = sizeof(InstancePositionType);
		strides[2] = sizeof(InstanceColorType);
		offsets[0] = 0;
		offsets[1] = 0;
		offsets[2] = 0;
		buffers[0] = vertexBuffer;
		//buffers[1] = instanceBuffer;
		buffers[1] = instanceBuffer[0];
		buffers[2] = instanceBuffer[1];

		D3D::GetDC()->IASetVertexBuffers(0, 3, buffers, strides, offsets);
		//D3D::GetDC()->IASetVertexBuffers(0, 2, buffers, strides, offsets);
		D3D::GetDC()->IASetPrimitiveTopology(topology);

		colorBuffer->SetPSBuffer(1); // 레지스터 번호
		worldBuffer->SetMatrix(World());
		worldBuffer->SetVSBuffer(1);
		instanceShader->Render();

		D3D::GetDC()->DrawInstanced(vertexCount, instanceCount, 0, 0);
	}
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
	D3DXCOLOR oldColor = colorBuffer->Data.Color;
	//if (color.r == oldColor.r && color.g == oldColor.g
	//	&& color.b == oldColor.b && color.a == oldColor.a)
	//	return;
	if (oldColor == color)
		return;

	colorBuffer->Data.Color = color;
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

void Line::CreateInstance()
{
	//if (positionInstances != NULL) 
	//	SAFE_DELETE_ARRAY(positionInstances);
	if (colorInstances != NULL)
		SAFE_DELETE_ARRAY(colorInstances);

	//positionInstances = new InstancePositionType[instanceCount];
	colorInstances = new InstanceColorType[instanceCount];

	for (int i = 0; i < instanceCount; i++) {
		//positionInstances[i].Position = positions[i];
		colorInstances[i].Color = colors[i];
	}

	//// CreateInstanceBuffer
	//{
	//	D3D11_BUFFER_DESC desc = { 0 };
	//	desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
	//	desc.ByteWidth = sizeof(InstancePositionType) * instanceCount; // 정점 버퍼에 들어갈 데이터의 크기
	//	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//	desc.CPUAccessFlags = 0;
	//	desc.MiscFlags = 0;
	//	desc.StructureByteStride = 0;

	//	D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
	//	data.pSysMem = positionInstances; // 쓸 데이터의 주소
	//	data.SysMemPitch = 0;
	//	data.SysMemSlicePitch = 0;

	//	HRESULT hr = D3D::GetDevice()->CreateBuffer(
	//		&desc, &data, &instanceBuffer[0]);
	//	assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	//}

	// CreateInstanceBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(InstanceColorType) * instanceCount; // 정점 버퍼에 들어갈 데이터의 크기
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
		data.pSysMem = colorInstances; // 쓸 데이터의 주소
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &instanceBuffer[1]);
		assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	}

	if (worldInstances != NULL)
		SAFE_DELETE_ARRAY(worldInstances);

	worldInstances = new InstanceWorldType[instanceCount];

	for (int i = 0; i < instanceCount; i++) {
		//worldInstances[i].World = worlds[i];
		D3DXMatrixTranspose(&worldInstances[i].World, &worlds[i]);
	}

	// CreateInstanceBuffer
	{
		D3D11_BUFFER_DESC desc = { 0 };
		desc.Usage = D3D11_USAGE_DEFAULT; // 어떻게 저장될지에 대한 정보
		desc.ByteWidth = sizeof(InstanceWorldType) * instanceCount; // 정점 버퍼에 들어갈 데이터의 크기
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA  data = { 0 }; // 얘를 통해서 값이 들어감 lock 대신
		data.pSysMem = worldInstances; // 쓸 데이터의 주소
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;

		HRESULT hr = D3D::GetDevice()->CreateBuffer(
			&desc, &data, &instanceBuffer[0]);
		assert(SUCCEEDED(hr)); // 성공되면 hr 0보다 큰 값 넘어옴
	}
}

void Line::SetWorlds(vector<D3DXMATRIX>& worlds)
{
	bInstance = true;
	this->worlds = worlds;
	instanceCount = worlds.size();
}

//void Line::SetPositions(vector<D3DXVECTOR4>& positions) {
//	bInstance = true;
//	this->positions = positions;
//	instanceCount = positions.size();
//}
//
void Line::SetColors(vector<D3DXCOLOR>& colors)
{
	bInstance = true;
	this->colors = colors;
	instanceCount = colors.size();
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
