#pragma once

enum class DrawType
{
	UP,
	DOWN,
	FLAT,
	TEXTURE,
	SELECT,
	OBJECT,
	LIGHT,
	END
};

class MapToolBrush
{
	friend class MapToolData;
public:
	MapToolBrush(class MapToolData* mapData);
	~MapToolBrush();

	void Draw();
	void SetRender();

	void ImGuiRender();

	DrawType GetDrawType() { return drawType; }
	void SetDrawType(DrawType type) { drawType = type; }

	void SetBrushType(int type) { brushBuffer->Data.BrushType = type; }
	int GetBrushType() { return brushBuffer->Data.BrushType; }

	D3DXVECTOR3 GetPosition() { return brushBuffer->Data.Location; }
	void SetPosition(D3DXVECTOR3& pos) { brushBuffer->Data.Location = pos; }

private:
	void TerrainUp(D3DXVECTOR3 & location);
	void TerrainDown(D3DXVECTOR3 & location);
	void TerrainFlat(D3DXVECTOR3 & location);
	void TerrainTexture(D3DXVECTOR3 & location);

	void CreateObject(D3DXVECTOR3 & location);
	void CreateLight(D3DXVECTOR3 & location);

	void LoadDrawTexture(wstring fileName);
	void SaveAlphaMap(wstring fileName);
	void LoadAlphaMap(wstring fileName);
	void SaveHeightMap(wstring fileName);
	void LoadHeightMap(wstring fileName);

private:
	class MapToolData* mapData;

	DrawType drawType;

	Texture* textures[5];
	string texName[5];

	function<void(std::wstring)> func;

	//Draw 관련 변수들
	float flatHeight;
	float drawConcentration;
	int selectTexture;
	//LightType selectLight;
	enum ObjType selectObject;
	int selectTap;

private:
	class BrushBuffer : public ShaderBuffer
	{
	public:
		BrushBuffer() : ShaderBuffer(&Data, sizeof(Data))
		{
			Data.BrushType = 1;
			Data.Location = D3DXVECTOR3(0, 0, 0);
			Data.Range = 2;
			Data.Color = D3DXVECTOR3(0, 1, 0);
		}

		struct Struct
		{
			int BrushType;
			D3DXVECTOR3 Location;

			int Range;
			D3DXVECTOR3 Color;
		} Data;
	};
	BrushBuffer* brushBuffer;
};