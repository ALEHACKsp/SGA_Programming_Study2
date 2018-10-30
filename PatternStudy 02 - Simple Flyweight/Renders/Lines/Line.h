#pragma once

class Line
{
public:
	//struct InstanceColorType
	//{
	//	D3DXCOLOR Color = D3DXCOLOR(1,1,1,1);
	//};
	//struct InstancePositionType
	//{
	//	D3DXVECTOR4 Position = D3DXVECTOR4(0,0,0,1);
	//};

	struct InstanceWorldType
	{
		D3DXMATRIX World;
	};

private:
	class ColorBuffer; // 내부 클래스라서 클래스 안에서 전방선언 해줘야함
protected:
	typedef Vertex VertexType;
public:
	Line();
	~Line();

	void Init(D3DXVECTOR3 * positions, UINT lineCount,
		D3DXCOLOR color = D3DXCOLOR(1, 1, 1, 1),
		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	virtual void Render();
	virtual void PostRender();

	void SetColor(D3DXCOLOR color);
	D3DXCOLOR GetColor() { return colorBuffer->Data.Color; }
	virtual void UpdateBuffer();

	void SetName(string name) { this->name = name; }
	string GetName() { return name; }

	void SetLine(D3DXVECTOR3 * positions, UINT lineCount, 
		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST, bool isChange = false);

public:
	D3DXMATRIX World();

	void Position(D3DXVECTOR3& vec);
	void Position(float x, float y, float z);
	D3DXVECTOR3 Position();

	void Scale(D3DXVECTOR3& vec);
	void Scale(float x, float y, float z);
	D3DXVECTOR3 Scale();

	void Rotation(D3DXVECTOR3& vec);
	void Rotation(float x, float y, float z);
	void RotationDegree(D3DXVECTOR3& vec);
	void RotationDegree(float x, float y, float z);
	D3DXVECTOR3 Rotation();
	D3DXVECTOR3 RotationDegree();

	D3DXVECTOR3 Direction();
	D3DXVECTOR3 Up();
	D3DXVECTOR3 Right();

	void CreateInstance();
	//void SetPositions(vector<D3DXVECTOR4>& positions);
	//void SetColors(vector<D3DXCOLOR>& colors);
	void SetWorlds(vector<D3DXMATRIX>& worlds);

private:
	void UpdateWorld();

private:
	D3DXMATRIX world;
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXVECTOR3 rotation;

	D3DXVECTOR3 direction;
	D3DXVECTOR3 up;
	D3DXVECTOR3 right;

private:
	Shader* shader;
	WorldBuffer * worldBuffer;

	//ID3D11Buffer * indexBuffer;
protected:
	ColorBuffer * colorBuffer;
	ID3D11Buffer * vertexBuffer;
	UINT vertexCount, indexCount;

	VertexType * vertices;
	//UINT * indices;

	D3D11_PRIMITIVE_TOPOLOGY topology;

	string name;

	bool bInstance;
	//vector<D3DXVECTOR4> positions;
	//vector<D3DXCOLOR> colors;
	vector<D3DXMATRIX> worlds;
	Shader* instanceShader;

	//ID3D11Buffer* instanceBuffer[2];
	ID3D11Buffer* instanceBuffer;
	UINT instanceCount;
	//InstancePositionType * positionInstances;
	//InstanceColorType * colorInstances;
	InstanceWorldType* worldInstances;
private:
	// 컬러버퍼 라이트로 쓸꺼
	class ColorBuffer : public ShaderBuffer
	{
	public:
		ColorBuffer()
			: ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXCOLOR(1, 1, 1, 1);
		}

		~ColorBuffer() 
		{

		}

		struct Struct
		{
			D3DXCOLOR Color;
		} Data;
	};
};