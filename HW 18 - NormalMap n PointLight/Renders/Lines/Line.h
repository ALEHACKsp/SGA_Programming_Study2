#pragma once

class Line
{
private:
	class ColorBuffer; // ���� Ŭ������ Ŭ���� �ȿ��� ���漱�� �������
protected:
	typedef VertexColor VertexType;
public:
	Line();
	~Line();

	void Init(D3DXVECTOR3 * positions, UINT lineCount,
		D3DXCOLOR color = D3DXCOLOR(1, 1, 1, 1),
		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	virtual void Render();
	virtual void PostRender();

	void SetColor(D3DXCOLOR color);
	virtual void UpdateBuffer();

	void SetName(string name) { this->name = name; }
	string GetName() { return name; }
private:
	Shader* shader;
	WorldBuffer * worldBuffer;

	//ID3D11Buffer * indexBuffer;
	
	ColorBuffer * colorBuffer;

protected:
	ID3D11Buffer * vertexBuffer;
	UINT vertexCount, indexCount;

	VertexType * vertices;
	//UINT * indices;

	D3D11_PRIMITIVE_TOPOLOGY topology;

	string name;
private:
	// �÷����� ����Ʈ�� ����
	class ColorBuffer : public ShaderBuffer
	{
	public:
		ColorBuffer()
			: ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Color = D3DXVECTOR4(1, 1, 1, 1);
		}

		~ColorBuffer() 
		{

		}

		struct Struct
		{
			D3DXVECTOR4 Color;
		} Data;
	};
};