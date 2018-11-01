#pragma once

class ParticleInstance
{
public:
	ParticleInstance(D3DXVECTOR3 position, D3DXVECTOR3 scale, D3DXQUATERNION orientation,
		class ParticleInstancer* instancer);
	ParticleInstance(D3DXVECTOR3 position, D3DXVECTOR3 scale, D3DXVECTOR3 mods,
		class ParticleInstancer* instancer);
	ParticleInstance(D3DXVECTOR3 position, D3DXVECTOR3 scale, 
		class ParticleInstancer* instancer);
	~ParticleInstance();

	void Update();

	void TranslateAA(D3DXVECTOR3 distance);
	bool Moved(D3DXVECTOR3 distance);

	int myID() { return id; }

private:
	static int ID;
	int id;

public:
	D3DXVECTOR3 position;
	D3DXVECTOR3 scale;
	D3DXQUATERNION orientation;

	D3DXMATRIX world;
	D3DXMATRIX aaWorld;

	class ParticleInstancer* instancer;

	float rnd;

	D3DXVECTOR3 pMods;
};