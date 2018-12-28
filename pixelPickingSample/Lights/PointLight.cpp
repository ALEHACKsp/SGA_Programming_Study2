#include "stdafx.h"
#include "PointLight.h"
#include "./Utilities/Gizmo.h"

PointLight::PointLight()
{
	buffer = new Buffer();
}

PointLight::~PointLight()
{
	SAFE_DELETE(buffer);
}

void PointLight::Update()
{
	buffer->SetPSBuffer(2);
}

void PointLight::GizmoRender()
{
}

void PointLight::GizmoRender(UINT index)
{
}
