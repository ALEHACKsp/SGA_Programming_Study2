#include "stdafx.h"
#include "ParticleInstance.h"

#include "ParticleInstancer.h"

int ParticleInstance::ID = 0;

ParticleInstance::ParticleInstance(D3DXVECTOR3 position, D3DXVECTOR3 scale, D3DXQUATERNION orientation, ParticleInstancer * instancer)
	: ParticleInstance::ParticleInstance(position, scale, instancer)
{
	this->orientation = orientation;
	Update();
}

ParticleInstance::ParticleInstance(D3DXVECTOR3 position, D3DXVECTOR3 scale, D3DXVECTOR3 mods, ParticleInstancer * instancer)
	: ParticleInstance::ParticleInstance(position, scale, instancer)
	
{
	this->pMods = mods;
	Update();
}

ParticleInstance::ParticleInstance(D3DXVECTOR3 position, D3DXVECTOR3 scale, ParticleInstancer * instancer)
	: instancer(instancer), position(position), scale(scale)
{
	ID++;
	id = ID;

	// instancer InstanceTransformMatrices.Add
	instancer->instanceTransformMatrices[this] = world;
	instancer->instances[myID()] = this;

	Update();
}

ParticleInstance::~ParticleInstance()
{
}

void ParticleInstance::Update()
{
	D3DXMATRIX S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationQuaternion(&R, &orientation);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	world = S * R * T;

	// Set the scale
	world._13 = scale.x;
	world._24 = scale.y;

	// set the image, alpha and color mod
	world._12 = pMods.x;
	world._23 = pMods.y;
	world._34 = pMods.z;

	// instancer instanceTransformMatrices = world
	instancer->instanceTransformMatrices[this] = world;
}

void ParticleInstance::TranslateAA(D3DXVECTOR3 distance)
{
	D3DXMATRIX matrix;
	D3DXMatrixIdentity(&matrix);

	D3DXVECTOR4 temp;
	D3DXVec3Transform(&temp, &distance, &matrix);
	position += D3DXVECTOR3(temp.x, temp.y, temp.z);
}

bool ParticleInstance::Moved(D3DXVECTOR3 distance)
{
	return distance != D3DXVECTOR3(0,0,0);
}
