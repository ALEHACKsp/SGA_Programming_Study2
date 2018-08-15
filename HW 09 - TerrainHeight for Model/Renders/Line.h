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

	void Render(D3DXVECTOR3 * positions, UINT lineCount, 
		D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
protected:
	Shader* shader;
	WorldBuffer * worldBuffer;

	ID3D11Buffer * vertexBuffer;
	//ID3D11Buffer * indexBuffer;

	UINT vertexCount, indexCount;
	
	VertexType * vertices;
	//UINT * indices;
	
	ColorBuffer * colorBuffer;

	//D3DXVECTOR3 * startPos;
	//D3DXVECTOR3 * endPos;
	UINT lineCount;
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