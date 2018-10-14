#include "stdafx.h"
#include "GlobalBuffers.h"

WorldBuffer::WorldBuffer() : ShaderBuffer(&Data, sizeof(Struct))
{
	D3DXMatrixIdentity(&Data.World);
}

void WorldBuffer::SetMatrix(D3DXMATRIX mat)
{
	Data.World = mat;
	D3DXMatrixTranspose(&Data.World, &Data.World);
}

ViewProjectionBuffer::ViewProjectionBuffer() : ShaderBuffer(&Data, sizeof(Struct))
{
	D3DXMatrixIdentity(&Data.View);
	D3DXMatrixIdentity(&Data.Projection);
}

void ViewProjectionBuffer::SetView(D3DXMATRIX mat)
{
	Data.View = mat;
	D3DXMatrixInverse(&Data.ViewInverse, NULL, &mat);

	D3DXMatrixTranspose(&Data.View, &Data.View);
	D3DXMatrixTranspose(&Data.ViewInverse, &Data.ViewInverse);
}

void ViewProjectionBuffer::SetProjection(D3DXMATRIX mat)
{
	Data.Projection = mat;
	D3DXMatrixTranspose(&Data.Projection, &Data.Projection);
}

LightBuffer::LightBuffer() : ShaderBuffer(&Data, sizeof(Struct))
{
	Data.Direction = D3DXVECTOR3(-1, -1, -1);
	Data.Color = D3DXCOLOR(1, 1, 1, 1);

	Data.Overcast = 1;
	Data.Intensity = 0.5f;
}
