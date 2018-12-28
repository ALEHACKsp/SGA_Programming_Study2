#include "stdafx.h"
#include "SpotLight.h"
#include "./Objects/GameRender.h"

SpotLight::SpotLight()
{
	buffer = new Buffer();
}

SpotLight::~SpotLight()
{
	SAFE_DELETE(buffer);
}

void SpotLight::Update()
{
	buffer->SetPSBuffer(3);
}

void SpotLight::Render()
{
}

void SpotLight::Render(UINT index)
{
}
