#pragma once

enum class MouseType
{
	NONE,
	BRUSH,
	SELECTED,
	END
};

enum LightType
{
	LIGHT_POINT,
	LIGHT_SPOT,
	LIGHT_AREA,
	LIGHT_END
};

class MapTool
{
public:
	MapTool(ExecuteValues* values);
	~MapTool();

	void Update();

	void PreRender();
	void Render();
	void PostRender();

	void ResizeScreen();

	float Y(D3DXVECTOR3& position);
	bool Y(OUT D3DXVECTOR3* out, D3DXVECTOR3& position);

	bool Picked(OUT D3DXVECTOR3* out);
	bool Pick2D(OUT D3DXVECTOR3* out);

private:
	void CreateBuffer(class MapToolData* mapData);

	void ImGuiRender();

	void ChangeObjectBrush(enum ObjType object);
	void MakeUndo();

	function<void(std::wstring)> func;
private:
	ExecuteValues* values;

	Material* material;
	ID3D11Buffer* vertexBuffer, *indexBuffer;

	class MapToolData* mapData;

	bool bWireframe;
	RasterizerState* rasterizer[2];

	class RayTracingRT* rayTracing;

private:
	//object
	vector<class GameModel*> objects;
	Texture* treeTex;
	class GameModel* objectBrush;

	class PointLight* pointLight;
	class SpotLight* spotLight;
	class AreaLight* areaLight;
	int lightBrush;
	int lightLast;

private:
	class MapToolCommandCaretaker* commands;
	class MapToolBrush* brush;
	MouseType mouseType;

	D3DXVECTOR3 mousePos;
	bool bMouseOnWindow;
	bool bPicked;
	bool bTransformed;

	class PickingObject* pickingObj;


private:
	class GridBuffer : public ShaderBuffer
	{
	public:
		GridBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.Type = 1;
			Data.Color = D3DXVECTOR3(1, 1, 1);

			Data.Spacing = 1;
			Data.Thickness = 0.05f;
		}

		struct Struct
		{
			int Type;
			D3DXVECTOR3 Color;

			int Spacing;
			float Thickness;

			float Padding[2];
		} Data;
	};
	GridBuffer* gridBuffer;
};