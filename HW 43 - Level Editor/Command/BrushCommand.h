#pragma once

#include "Command.h"

class BrushCommand : public Command
{
public:
	BrushCommand(class TerrainRender* terrain);
	~BrushCommand();

	// Command을(를) 통해 상속됨
	virtual void Execute() override;
	virtual void Undo() override;
	virtual void Render() override;

private:
	class TerrainRender* terrain;

	ID3D11Texture2D* prevHeightMapTex;
	ID3D11ShaderResourceView* prevHeightMapSRV;
	ID3D11Texture2D* heightMapTex;
	ID3D11ShaderResourceView* heightMapSRV;

	int id;
};