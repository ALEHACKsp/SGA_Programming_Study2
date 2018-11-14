#pragma once

class Ray
{
public:
	Ray(D3DXVECTOR3& origin, D3DXVECTOR3 direction, float length = 1000.0f, bool bDebug = false);
	~Ray();

	void Render();
	void PostRender();

	void Get(D3DXVECTOR3* origin, D3DXVECTOR3* direction) {
		*origin = this->origin;
		*direction = this->direction;
	}

	void Set(D3DXVECTOR3& origin, D3DXVECTOR3& direction);

	void SetLength(float length) { this->length = length; }
	float GetLength() { return length; }

	void SetDebug(bool bDebug) { this->bDebug = bDebug; }
	bool GetDebug() { return bDebug; }

private:
	D3DXVECTOR3 origin;
	D3DXVECTOR3 direction;

	bool bDebug;
	float length;

	class DebugDrawLine* debug;
};